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

#include <FI_injector.h>
#include <FI_result.h>
#include <FI_result.h>
#include <FI_defines.h>

FI_injector::FI_injector()
{

}

FI_injector::FI_injector(char *targetLocation, int startupDelay, int burstTime, int burstFrequency)
{
    m_targetLocation = targetLocation;    
    m_startupDelay = startupDelay * 1000;
    m_burstTime = burstTime;
    m_burstFrequency = burstFrequency;
}

FI_injector::~FI_injector()
{

}

void FI_injector::run_injection()
{   
    // Seed the random number generator
    std::srand(std::time(nullptr));

    // Start the target process
    start_process();

    usleep(m_startupDelay);

    // Record the start time before entering the outer while loop        
    long temp_time = 0;

    while (is_process_running()) 
    {
        auto burst_start_time = std::chrono::steady_clock::now();

        get_random_core();

        while (burst_active(burst_start_time)) 
        {
            // Pause all child processes

            if (get_target_process())
            {
                inject_fault();
                                
                long time = current_time_in_ms() - get_start_time();

                if (time - temp_time > 10) 
                {
                    add_result(time);
                    temp_time = time;
                }

                usleep(1000);
            }            
        }
        usleep(m_burstFrequency);
    }
}

pid_t FI_injector::start_process()
{
    pid_t pid = fork();

    if (pid == 0) 
    {        
        execl(m_targetLocation, m_targetLocation, (char *)NULL);
        perror("execl");
        exit(1);
    }
    else if (pid > 0) 
    {
        m_target = pid;
        m_startTime = current_time_in_ms();
        return pid;
    }
    else
    {
        perror("fork");
        return -1;
    }
}

bool FI_injector::is_process_running() 
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

void FI_injector::inject_fault()
{
    get_random_register();

    // Read the registers
    struct user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, m_process, nullptr, &regs) == -1) 
    {
        ptrace(PTRACE_DETACH, m_process, nullptr, nullptr);
        return;
    }

    // Perform the bit flip on the specified register
#ifndef GOLDEN_RUN
    flip_bit(m_register, regs);
    for (auto& t : m_targets)
        if (!m_targetName.compare(t.name))
            t.injections++;
#endif

    // Write the modified registers back to the process
    if (ptrace(PTRACE_SETREGS, m_process, nullptr, &regs) == -1) 
    {
        fprintf(stderr, "Failed to set registers: %s\n", strerror(errno));
        ptrace(PTRACE_DETACH, m_process, nullptr, nullptr);
        return;
    }

    for (auto child : m_childPIDs)
    {
        ptrace(PTRACE_DETACH, child, nullptr, nullptr);
        ptrace(PTRACE_CONT, child, nullptr, nullptr);
    }

    // // Detach from the process and allow it to continue
    // if (ptrace(PTRACE_DETACH, m_process, nullptr, nullptr) == -1) 
    // {
    //     fprintf(stderr, "Failed to detach from process: %s\n", strerror(errno));
    //     return;
    // }

    // // Allow the process to continue execution
    // if (ptrace(PTRACE_CONT, m_process, nullptr, nullptr) == -1)
    //     return;
}

bool FI_injector::get_child_PIDs()
{
    m_childPIDs.clear();

    vector<pid_t> childPIDs;
    string path = "/proc/" + to_string(m_target) + "/task/" + to_string(m_target) + "/children";
    ifstream file(path);
    if (!file.is_open())
        return false;

    pid_t childPID;
    while (file >> childPID)
        m_childPIDs.push_back(childPID);

    return true; 
}

int FI_injector::get_core_of_process(pid_t process)
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

bool FI_injector::get_target_process()
{
    int status;
    if (!get_child_PIDs())
        return false;
    
    m_process = 0;

    for (int childPID : m_childPIDs) 
    {
        int childCore = get_core_of_process(childPID);

        if (childCore == m_core) 
        {
            m_process = childPID;
            get_process_name();
            break;
        }
    }

    if (!m_process)
        return false;

    // Attach to the process
    if (ptrace(PTRACE_ATTACH, m_process, nullptr, nullptr) == -1)
        return false;

    // Wait for the process to stop    
    if (waitpid(m_process, &status, 0) == -1) 
    {
        ptrace(PTRACE_DETACH, m_process, nullptr, nullptr);
        return false;
    }

    if (!WIFSTOPPED(status)) 
    {
        ptrace(PTRACE_DETACH, m_process, nullptr, nullptr);
        return false;
    }

    return true;
}

void FI_injector::flip_bit(intel_registers reg, struct user_regs_struct &regs)
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
            ptrace(PTRACE_DETACH, m_process, nullptr, nullptr);
            return;
    }
}

void FI_injector::get_random_core()
{
    m_core = std::rand() % NUM_OF_CORES;
}

void FI_injector::get_random_register()
{
    int randomValue = std::rand() % RANDOM;

    m_register = static_cast<intel_registers>(randomValue);
}

bool FI_injector::get_random_child_pid()
{
    char path[256];
    int status;
    string task_name;

    // Construct the path to the children file
    snprintf(path, sizeof(path), "/proc/%d/task/%d/children", m_target, m_target);

    // Get the pid of a random child
    if (!get_random_PID(path))
        return false;

    if (m_core != get_core_of_child_process())
        return false;

    // Get the name of the target process
    get_process_name();

    // Attach to the process
    if (ptrace(PTRACE_ATTACH, m_process, nullptr, nullptr) == -1)
        return false;

    // Wait for the process to stop    
    if (waitpid(m_process, &status, 0) == -1) 
    {
        ptrace(PTRACE_DETACH, m_process, nullptr, nullptr);
        return false;
    }

    if (!WIFSTOPPED(status)) 
    {
        ptrace(PTRACE_DETACH, m_process, nullptr, nullptr);
        return false;
    }

    return true;
}

bool FI_injector::get_random_PID(char* path)
{
    pid_t pid;
    std::vector<pid_t> child_pids;    
    int pid_index;

    // Open the children file
    std::ifstream children_file(path);
    if (!children_file)
        return false; 

    // Read all child PIDs into a vector
    while (children_file >> pid)
        child_pids.push_back(pid);
    
    children_file.close();

    // Check if there are any child PIDs
    if (child_pids.empty())
        return false;    

    // Check if there is more than one child PID
    if (child_pids.size() == 1) 
    {
        m_process = child_pids[0];
        return true;
    }

    // Get a random child PID index
    pid_index = std::rand() % child_pids.size();

    m_process = child_pids[pid_index];

    return true;
}

bool FI_injector::get_process_name()
{
    char path[256];

    // Construct the path to the comm file
    snprintf(path, sizeof(path), "/proc/%d/comm", m_process);

    // Open the comm file to read the task name
    std::ifstream comm_file(path);
    if (!comm_file)
        return false;

    // Read the task name
    string task_name;
    getline(comm_file, task_name);
    comm_file.close();

    m_targetName = task_name;

    return true;
}

int FI_injector::get_core_of_child_process() {
    // Construct the path to the stat file
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/stat", m_process);

    // Open the stat file
    std::ifstream stat_file(path);
    if (!stat_file)
        return -1;

    // Read the entire contents of the stat file
    std::string stat_contents;
    std::getline(stat_file, stat_contents);
    stat_file.close();

    // Split the contents by spaces
    std::istringstream iss(stat_contents);
    std::vector<std::string> stat_fields;
    std::string field;
    while (iss >> field)
        stat_fields.push_back(field);

    // Check if we have at least 39 fields
    if (stat_fields.size() < 39)
        return -1;

    // Return the 39th field which is the CPU core number
    int core = std::stoi(stat_fields[38]);
    return core;
}

bool FI_injector::burst_active(const std::chrono::steady_clock::time_point& start_time)
{
    auto current_time = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
    return elapsed_time < m_burstTime;
}

void FI_injector::add_result(long time)
{
    FI_result *r = new FI_result(m_core, time, m_targets , m_targetName);

    m_results.push_back(r);
}

long current_time_in_ms() 
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    return (spec.tv_sec * 1000) + (spec.tv_nsec / 1000000);
}

void FI_injector::write_results_to_file()
{
    string injection_results = generateOutputString("FI_results/injection_results/injection");
    FILE *injection_file = fopen(injection_results.c_str(), "w");

    string target_results = generateOutputString("FI_results/target_results/targets");
    FILE *target_file = fopen(target_results.c_str(), "w");

    if (!injection_file || !target_file)
    {
        perror("Failed to open file");
        return;
    }

    // Create column names for injection_file
    fprintf(injection_file, "time\t");
    fprintf(target_file, "time\t");

    for (int i = 0; i < NUM_OF_CORES; i++)
    {
        fprintf(injection_file, "Core %d", m_cores[i]);
        if (i < NUM_OF_CORES - 1)
            fprintf(injection_file, "\t");
    }

    fprintf(injection_file, "\n");

    // create column names for target_file
    for (size_t i = 0; i < m_targets.size(); i++)
    {
        fprintf(target_file, "%s", m_targets[i].name.c_str());

        if (i < m_targets.size() - 1)
            fprintf(target_file, "\t");        
    }

    fprintf(target_file, "\n");

    // Declare the arrays to store the results
    int injection_sum[NUM_OF_CORES];
    int target_sum[m_targets.size()];

    // Initialize the array results
    for (size_t i = 0; i < NUM_OF_CORES; i++) injection_sum[i] = 0;
    for (size_t i = 0; i < m_targets.size(); i++) target_sum[i] = 0; 

    // Process the results
    for (auto& result: m_results)
    {
        fprintf(injection_file, "%ld\t", result->get_time());
        fprintf(target_file, "%ld\t", result->get_time());

        for (size_t i = 0; i < NUM_OF_CORES; i++) 
        {
            if (result->get_target() == m_cores[i])
            {
                injection_sum[i]++;
                break;
            }
        }

        for (size_t i = 0; i < NUM_OF_CORES; i++)
        {
            fprintf(injection_file, "%d", injection_sum[i]);
            if (i < NUM_OF_CORES - 1)            
                fprintf(injection_file, "\t");
            
        }

        for (size_t i = 0; i < m_targets.size(); i++)
        {
            if (result->get_targets()[i])
                target_sum[i]++;
        }

        for (size_t i = 0; i < m_targets.size(); i++)
        {
            fprintf(target_file, "%d", target_sum[i]);
            if (i < m_targets.size() - 1)
                fprintf(target_file, "\t");
        }

        fprintf(injection_file, "\n");
        fprintf(target_file, "\n");
    }

    fclose(injection_file);
    fclose(target_file);
}

void FI_injector::print_results()
{
    printf("*** Injection results ***\n");

    for (size_t i = 0; i < m_targets.size(); i++)
        printf("%s: \t injections: %d\n", m_targets[i].name.c_str(), m_targets[i].injections);
}

string generateOutputString(const string& prefix) {
    // Get the current time
    std::time_t now = std::time(nullptr);
    std::tm timeinfo = *std::localtime(&now);

    // Format the date and time
    std::ostringstream oss;
    oss << std::put_time(&timeinfo, "%Y-%m-%d_%H-%M-%S");
    string suffix = oss.str();

    // Construct the output string
    string output = prefix + "_" + suffix + ".txt";

    return output;
}
