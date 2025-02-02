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

#include <algorithm>    // for the all of function

#include <FI_campaign.h>
#include <FI_result.h>
#include <FI_target.h>
#include <FI_defines.h>
#include <FI_logger.h>

FI_campaign::FI_campaign(string targetLocation, string outputDirectory, int startupDelay, int burstTime, int burstFrequency, int injectionDelay, bool goldenRun)
{
    m_targetLocation = targetLocation;
    m_outputDirectory = outputDirectory;
    m_startupDelay = startupDelay * MILISECOND;
    m_burstTime = burstTime;
    m_baseBurstTime = burstTime;
    m_burstFrequency = burstFrequency;
    m_baseBurstFrequency = burstFrequency;
    m_injectionDelay = injectionDelay * MILISECOND;
    m_injector = new FI_injector(goldenRun);
    m_logger = new FI_logger();
    m_logger->set_result_directory(outputDirectory);
}

FI_campaign::~FI_campaign()
{

}

void FI_campaign::init_campaign(int injectorCore, int numOfTargets, vector<int>targetCores)
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

void FI_campaign::run_injection()
{   
    // Seed the random number generator
    std::srand(std::time(nullptr));

    // Start the target process
    start_process();

    // Give the target time to initialize
    usleep(m_startupDelay);    

    while (active())
    {
        auto burst_start_time = std::chrono::steady_clock::now();
        
        get_random_cores();        

        while (burst_active(burst_start_time)) 
        {
            if (!stop_targets()) continue;

            create_targets(burst_start_time);

            m_injector->inject_faults(m_logger);            

            start_targets();

            usleep(m_injectionDelay);
        }
        
        FI_pause();

        apply_random_frequency_deviation();
    }

    m_logger->output_tsv(this);

    cleanup_campaign();
}

void FI_campaign::cleanup_campaign()
{
    m_logger->cleanup_logger();
    delete m_logger;

    delete m_injector;

    for (FI_target* target : m_targets)
    {
        delete target;
    }
}

bool FI_campaign::stop_targets()
{
    if (!get_target_PIDs())
        return false;

    int status;

    // Pause the scheduler
    ptrace(PTRACE_ATTACH, m_target, nullptr, nullptr);
    waitpid(m_target, &status, 0);

    if (!WIFSTOPPED(status))
    {
        ptrace(PTRACE_DETACH, m_target, nullptr, nullptr);
        return false;
    }    
    
    for (auto*& target : m_targets)    
    {
        // If pid is not set (0), skip the iteration
        if (!target->get_pid())
            continue;
        

        // Attach to the child process
        if (ptrace(PTRACE_ATTACH, target->get_pid(), nullptr, nullptr) == -1)
        {
            continue;
        }

        
        // Wait for the process to stop
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

    return true; // All children successfully stopped
}

bool FI_campaign::start_targets()
{
    kill(m_target, SIGCONT);
    ptrace(PTRACE_DETACH, m_target, nullptr, nullptr);

    for (auto*& target : m_targets) 
    {
        if (!target->get_pid())
            continue;

        // Send SIGCONT to ensure the process resumes execution
        //if (ptrace(PTRACE_CONT, target->get_pid(), nullptr, nullptr) == -1)
        if (kill(target->get_pid(), SIGCONT) == -1)
        {
            continue;
        }

        // Detach from the process
        if (ptrace(PTRACE_DETACH, target->get_pid(), nullptr, nullptr) == -1)
        {
            continue;
        }
    }

    return true; // All children successfully resumed
}

pid_t FI_campaign::start_process()
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

bool FI_campaign::active() 
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

void FI_campaign::create_targets(const chrono::steady_clock::time_point& start_time)
{
    m_injector->clear_jobs();

    for (size_t i = 0; i < m_target_cores.size(); i++)
    {
        // Find a task whose core matches the selected core        
        for (auto*& target : m_targets)    
        {
            if (target->get_core() == m_target_cores[i])
            {
                // // Add the target using FI_injector               
                m_injector->add_job(FI_job::declare_job(target->get_name(), target->get_pid(), target->get_core(), get_random_register()));
            }
        }
    }
}

bool FI_campaign::get_target_PIDs()
{
    bool result = false;

    for (auto*& t : m_targets)
    {
        t->set_pid(0);
    }

    string taskPath = "/proc/" + to_string(m_target) + "/task/";

    //Now add children tasks
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
                std::getline(childCommFile, childTaskName); // Read the child's task name
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

int FI_campaign::get_core_of_process(pid_t process)
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

void FI_campaign::flip_bit(intel_registers reg, struct user_regs_struct &regs)
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
            //ptrace(PTRACE_DETACH, m_process, nullptr, nullptr);
            return;
    }
}

void FI_campaign::get_random_cores()
{
    vector<int> availableCores = m_cores;
    random_shuffle(availableCores.begin(), availableCores.end());
 
    for (size_t i = 0; i < m_target_cores.size() && i < m_cores.size(); i++) 
    {
        m_target_cores[i] = availableCores[i];
    }

}

intel_registers FI_campaign::get_random_register()
{
    int randomValue = std::rand() % RANDOM;

    return static_cast<intel_registers>(randomValue);
}

bool FI_campaign::burst_active(const chrono::steady_clock::time_point& start_time)
{
    auto current_time = chrono::steady_clock::now();
    auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(current_time - start_time).count();        

    if (elapsed_time > m_burstTime)
    {
        // Change m_burstTime by a random ±10% based on the original BURST_TIME
        const int deviation = m_baseBurstTime / 4; // 10% of BURST_TIME
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> dist(-deviation, deviation); // Range: [-10%, +10%]
        
        m_burstTime = m_baseBurstTime + dist(gen); // Apply the random deviation to BURST_TIME

        return false;
    }

    return true;
}

// Function to apply ±10% random deviation to the frequency
void FI_campaign::apply_random_frequency_deviation()
{    
    const int deviation = m_baseBurstFrequency / 2; // 10% of the base frequency
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(-deviation, deviation); // Range: [-10%, +10%]
    m_burstFrequency = m_baseBurstFrequency + dist(gen);
    //return baseFrequency + dist(gen);
}
