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
#include <sstream>
#include <FI_injector.h>
#include <FI_result.h>
#include <FI_defines.h>

FI_injector::FI_injector()
{

}

FI_injector::FI_injector(char *processLocation)
{
    m_processLocation = processLocation;
    m_threadID = -1;                // Will be used for random injection
    m_burstTime = 100;
}

FI_injector::~FI_injector()
{

}

// Starts a process specified
pid_t FI_injector::start_process()
{
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execl(m_processLocation, m_processLocation, (char *)NULL); // Correctly pass m_process twice
        perror("execl");
        exit(1);
    } else if (pid > 0) {
        // Parent process
        m_process = pid;
        return pid;
    } else {
        // Fork failed
        perror("fork");
        return -1;
    }
}

bool FI_injector::is_process_running() {
    // Use the kill function with signal 0 to check if the process is running
    int status;
    int result = waitpid(m_process, &status, WNOHANG);
    if (result == 0) {
        return true; // Process exists
    } else if (result == -1) {
        perror("waitpid");
        return false; // Error
    } else {
        return false; // Process does not exist
    }
}

// char* FI_injector::get_register(FI_injector::intel_registers reg)
// {
//     switch(reg) {
//         case RAX: return "rax";
//         case RBX: return "rbx";
//         case RCX: return "rcx";
//         case RDX: return "rdx";
//         case RSI: return "rsi";
//         case RDI: return "rdi";
//         case RSP: return "rsp";
//         case RBP: return "rbp";
//         case R8: return "r8";
//         case R9: return "r9";
//         case R10: return "r10";
//         case R11: return "r11";
//         case R12: return "r12";
//         case R13: return "r13";
//         case R14: return "r14";
//         case R15: return "r15";
//         default: return "unknown";
//     }
// }

void FI_injector::inject_fault()
{
    // Read the registers
    struct user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, m_thread, nullptr, &regs) == -1) {
        ptrace(PTRACE_DETACH, m_thread, nullptr, nullptr);
        return;
    }

    // Perform the bit flip on the specified register
#ifndef GOLDEN_RUN
    flip_bit(m_register, regs);
#endif

    //printf("Injected in: %s \t pid: %d \t register: %s\n", m_name.c_str(), m_thread, get_register(m_register));

    // Write the modified registers back to the process
    if (ptrace(PTRACE_SETREGS, m_thread, nullptr, &regs) == -1) {
        fprintf(stderr, "Failed to set registers: %s\n", strerror(errno));
        ptrace(PTRACE_DETACH, m_thread, nullptr, nullptr);
        return;
    }

    // Detach from the process and allow it to continue
    if (ptrace(PTRACE_DETACH, m_thread, nullptr, nullptr) == -1) {
        fprintf(stderr, "Failed to detach from process: %s\n", strerror(errno));
        return;
    }

    // Allow the process to continue execution
    if (ptrace(PTRACE_CONT, m_thread, nullptr, nullptr) == -1) {
        //fprintf(stderr, "Failed to continue process: %s\n", strerror(errno));
        return;
    }
}

void FI_injector::flip_bit(FI_injector::intel_registers reg, struct user_regs_struct &regs)
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
            ptrace(PTRACE_DETACH, m_thread, nullptr, nullptr);
            return;
    }
}

void FI_injector::get_random_core()
{
    m_core = (std::rand() % NUM_OF_CORES) + 1;
}

void FI_injector::get_random_register()
{
    int randomValue = std::rand() % RANDOM;

    m_register = static_cast<intel_registers>(randomValue);
}

bool FI_injector::get_random_child_pid()
{
     // Construct the path to the children file
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/task/%d/children", m_process, m_process);

    // Open the children file
    std::ifstream children_file(path);
    if (!children_file) {
        perror("Failed to open children file");
        return false;
    }

    // Read all child PIDs into a vector
    std::vector<pid_t> child_pids;
    pid_t pid;
    while (children_file >> pid) {
        child_pids.push_back(pid);
    }
    children_file.close();

    // Check if there are any child PIDs
    if (child_pids.empty()) {
        return false;
    }

    // Generate a random index to select a PID from the vector
    std::random_device rd;  // Seed for the random number engine
    std::mt19937 gen(rd()); // Mersenne Twister random number engine
    std::uniform_int_distribution<> distrib(0, child_pids.size() - 1);

    pid_t random_child_pid = child_pids[distrib(gen)];

    if (m_core != get_core_of_child_process(random_child_pid)) {
        return false;
    }

    m_thread = random_child_pid;

    // Construct the path to the comm file
    snprintf(path, sizeof(path), "/proc/%d/comm", m_thread);

    // Attach to the process
    if (ptrace(PTRACE_ATTACH, m_thread, nullptr, nullptr) == -1) {
        return false;
    }

    // Wait for the process to stop
    int status;
    if (waitpid(m_thread, &status, 0) == -1) {
        ptrace(PTRACE_DETACH, m_thread, nullptr, nullptr);
        return false;
    }

    if (!WIFSTOPPED(status)) {
        ptrace(PTRACE_DETACH, m_thread, nullptr, nullptr);
        return false;
    }

    return true;
}

int FI_injector::get_core_of_child_process(pid_t child_pid) {
    // Construct the path to the stat file
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/stat", child_pid);

    // Open the stat file
    std::ifstream stat_file(path);
    if (!stat_file) {
        //perror("Failed to open stat file");
        return -1;
    }

    // Read the entire contents of the stat file
    std::string stat_contents;
    std::getline(stat_file, stat_contents);
    stat_file.close();

    // Split the contents by spaces
    std::istringstream iss(stat_contents);
    std::vector<std::string> stat_fields;
    std::string field;
    while (iss >> field) {
        stat_fields.push_back(field);
    }

    // Check if we have at least 39 fields
    if (stat_fields.size() < 39) {
        //fprintf(stderr, "Unexpected stat file format\n");
        return -1;
    }

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

bool FI_injector::contains(int target)
{
    for(int i = 0; i < NUM_OF_TARGETS; i++)
    {
        if (m_cores[i] == target)
        {
            return true;
        }
    }

    return false;
}

void FI_injector::add_result(time_t t)
{
    // FI_result(std::vector<int> cores, int target, time_t time);
    FI_result result(get_active_cores(), m_core, t);
    m_results.push_back(result);
    // for(const auto& r : m_results)
    // {
    //     printf("%ld \n", r.get_time);
    // }
}

std::vector<int> FI_injector::get_active_cores()
{
     // Construct the path to the children file
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/task/%d/children", m_process, m_process);

    // Open the children file
    std::ifstream children_file(path);
    if (!children_file) {
        perror("Failed to open children file");
        return {};
        //return false;
    }

    // Read all child PIDs into a vector
    std::vector<int> cores;
    pid_t pid;
    while (children_file >> pid)
    {
        cores.push_back(get_core_of_child_process(pid));
    }

    children_file.close();

    return cores;
}

void FI_injector::write_results_to_file()
{
    std::ofstream file("output.txt", std::ios::out | std::ios::trunc);
    if (!file) {
        std::cerr << "Failed to open or create file for writing: " << "output.txt" << std::endl;
        return;
    }

    // Write header
    file << "time\ttarget core";
    for (int i = 0; i < NUM_OF_CORES; ++i) {
        file << "\tcore " << i;
    }
    file << "\n";

    // std::vector<FI_result> get_results() { return m_results; }

    // Write results
    for (const auto& result : m_results) {
        int target = result.get_target();

        if (!target) {
            file << result.get_time() << " seconds" << "\t" << "\t";
        }
        else {
            file << result.get_time() << " seconds" << "\t" << result.get_target() << "\t";
        }


        std::vector<int> core_counters(NUM_OF_CORES, 0);
        for (int core : result.get_cores()) {
            if (core >= 0 && core < NUM_OF_CORES) {
                core_counters[core]++;
            }
        }

        for (int counter : core_counters) {
            file << "\t" << counter;
        }
        file << "\n";
    }

    file.close();
}