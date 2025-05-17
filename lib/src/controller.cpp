#include <iostream>
#include <cstdlib>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <random>
#include <string>
#include <ctime>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <memory> 
#include <sys/stat.h>
#include <algorithm>

#include <controller.h>
#include <result.h>
#include <target.h>
#include <defines.h>
#include <logger.h>

Controller::Controller(string targetLocation, string outputDirectory, int startupDelay, int burstTime, int burstDelay, int injectionDelay, bool goldenRun)
{
    m_targetLocation = targetLocation;
    m_outputDirectory = outputDirectory;
    m_startupDelay = startupDelay * MILISECOND;
    m_burstTime = burstTime;
    m_baseBurstTime = burstTime;
    m_burstDelay = burstDelay;
    m_baseBurstDelay = burstDelay;
    m_injectionDelay = injectionDelay * MILISECOND;
    m_injector = new Injector(goldenRun);
    m_logger = new Logger();
    m_logger->set_result_directory(outputDirectory);
}

Controller::~Controller()
{    
}

Controller* Controller::declare_controller(string targetLocation, string outputDirectory, int startupDelay, int burstTime, int burstFrequency, int injectionDelay, bool goldenRun)
{
    Controller* controller = new Controller(targetLocation, outputDirectory, startupDelay, burstTime, burstFrequency, injectionDelay, goldenRun);

    return controller;
}

void Controller::init_controller(int injectorCore, int numOfTargets, vector<int>targetCores)
{
    // Switch cores        
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);    
    CPU_SET(injectorCore, &cpuset);

    if (sched_setaffinity(0, sizeof(cpuset), &cpuset) != 0) 
    {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }

    m_cores = targetCores;
    m_target_cores.resize(numOfTargets);
}

void Controller::run_injection()
{
    std::srand(std::time(nullptr));

    start_process();
    
    usleep(m_startupDelay);    

    while (active())
    {
        auto burst_start_time = std::chrono::steady_clock::now();
        
        get_random_cores();        

        while (burst_active(burst_start_time)) 
        {
            if (!stop_targets()) continue;

            create_targets();

            m_injector->inject_faults(m_logger);            

            start_targets();

            usleep(m_injectionDelay);
        }
        
        pause();

        apply_random_frequency_deviation();
    }

    m_logger->output_tsv(this);

    cleanup_controller();
}

void Controller::cleanup_controller()
{
    m_logger->cleanup_logger();
    delete m_logger;

    delete m_injector;

    for (Target* target : m_targets)
    {
        delete target;
    }
}

bool Controller::stop_targets()
{
    if (!get_target_PIDs())
        return false;

    int status;

    ptrace(PTRACE_ATTACH, m_target, nullptr, nullptr);
    waitpid(m_target, &status, 0);

    if (!WIFSTOPPED(status))
    {
        ptrace(PTRACE_DETACH, m_target, nullptr, nullptr);
        return false;
    }    
    
    for (auto*& target : m_targets)    
    {
        if (!target->get_pid())
            continue;

        if (ptrace(PTRACE_ATTACH, target->get_pid(), nullptr, nullptr) == -1)
        {
            continue;
        }

        if (waitpid(target->get_pid(), &status, 0) == -1)
        {            
            ptrace(PTRACE_DETACH, target->get_pid(), nullptr, nullptr);
            continue;
        }

        if (!WIFSTOPPED(status))
        {
            ptrace(PTRACE_DETACH, target->get_pid(), nullptr, nullptr);
            continue;
        }
    }

    return true;
}

bool Controller::start_targets()
{
    kill(m_target, SIGCONT);
    ptrace(PTRACE_DETACH, m_target, nullptr, nullptr);

    for (auto*& target : m_targets) 
    {
        if (!target->get_pid())
            continue;

        if (kill(target->get_pid(), SIGCONT) == -1)
        {
            continue;
        }
        
        if (ptrace(PTRACE_DETACH, target->get_pid(), nullptr, nullptr) == -1)
        {
            continue;
        }
    }

    return true;
}

pid_t Controller::start_process()
{
    pid_t pid = fork();

    if (pid == 0) 
    {        
        execl(m_targetLocation.c_str(), m_targetLocation.c_str(), (char *)NULL);
        perror("execl");
        exit(1);
    }
    else if (pid > 0) 
    {
        m_target = pid;
        m_startTime = current_time_in_ms();
        m_injector->set_startTime(m_startTime);
        return pid;
    }
    else
    {
        perror("fork");
        return -1;
    }
}

bool Controller::active() 
{    
    int status;
    int result = waitpid(m_target, &status, WNOHANG);

    if (result == 0)
        return true;
    else if (result == -1)
    {
        perror("waitpid");
        return false;
    }
    else
        return false;
}

void Controller::create_targets()
{
    m_injector->clear_jobs();

    for (size_t i = 0; i < m_target_cores.size(); i++)
    {
        for (auto*& target : m_targets)    
        {
            if (target->get_core() == m_target_cores[i])
            {
                m_injector->add_job(Job::declare_job(target->get_name(), target->get_pid(), target->get_core(), get_random_register()));
            }
        }
    }
}

bool Controller::get_target_PIDs()
{
    bool result = false;

    for (auto*& t : m_targets)
    {
        t->set_pid(0);
    }

    string taskPath = "/proc/" + to_string(m_target) + "/task/";

    string childrenPath = taskPath + to_string(m_target) + "/children";
    std::ifstream childrenFile(childrenPath);
    if (childrenFile.is_open())
    {
        pid_t pid;
        while (childrenFile >> pid)
        {
            string childTaskPath = "/proc/" + to_string(pid) + "/comm";
            std::ifstream childCommFile(childTaskPath);
            if (childCommFile.is_open())
            {
                string childTaskName;
                std::getline(childCommFile, childTaskName);
                childCommFile.close();

                for (auto*& target : m_targets)
                {
                    if (target->get_name() == childTaskName)
                    {
                        int core = get_core_of_process(pid);
                        target->set_core(core);
                        target->set_pid(pid);

                        result = true;
                    }
                }
            }
        }
    }

    return result;
}

int Controller::get_core_of_process(pid_t process)
{
    cpu_set_t cpuSet;
    CPU_ZERO(&cpuSet);
    if (sched_getaffinity(process, sizeof(cpuSet), &cpuSet) == -1)
        return -1;

    for (int i = 0; i < CPU_SETSIZE; ++i)
        if (CPU_ISSET(i, &cpuSet))
            return i;

    return -1;
}

void Controller::flip_bit(intel_registers reg, struct user_regs_struct &regs)
{
    switch (reg) {
        case RAX: regs.rax ^= 0x1; break;
        case RBX: regs.rbx ^= 0x1; break;
        case RCX: regs.rcx ^= 0x1; break;
        case RDX: regs.rdx ^= 0x1; break;
        case RSI: regs.rsi ^= 0x1; break;
        case RDI: regs.rdi ^= 0x1; break;
        case RSP: regs.rsp ^= 0x1; break;
        case RBP: regs.rbp ^= 0x1; break;
        case R8: regs.r8 ^= 0x1; break;
        case R9: regs.r9 ^= 0x1; break;
        case R10: regs.r10 ^= 0x1; break;
        case R11: regs.r11 ^= 0x1; break;
        case R12: regs.r12 ^= 0x1; break;
        case R13: regs.r13 ^= 0x1; break;
        case R14: regs.r14 ^= 0x1; break;
        case R15: regs.r15 ^= 0x1; break;
        default:
            std::cerr << "Invalid register." << std::endl;
            return;
    }
}

void Controller::get_random_cores()
{
    std::vector<int> availableCores = m_cores;

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(availableCores.begin(), availableCores.end(), g);

    for (size_t i = 0; i < m_target_cores.size() && i < m_cores.size(); i++) 
    {
        m_target_cores[i] = availableCores[i];
    }
}

intel_registers Controller::get_random_register()
{
    int randomValue = std::rand() % RANDOM;

    return static_cast<intel_registers>(randomValue);
}

bool Controller::burst_active(const chrono::steady_clock::time_point& start_time)
{
    auto current_time = chrono::steady_clock::now();
    auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(current_time - start_time).count();        

    if (elapsed_time > m_burstTime)
    {
        const int deviation = m_baseBurstTime / 10;
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> dist(-deviation, deviation);
        
        m_burstTime = m_baseBurstTime + dist(gen);

        return false;
    }

    return true;
}

void Controller::apply_random_frequency_deviation()
{    
    const int deviation = m_baseBurstDelay / 10;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(-deviation, deviation);
    m_burstDelay = m_baseBurstDelay + dist(gen);
}
