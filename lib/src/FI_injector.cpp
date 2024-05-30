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
#include <ctime>
#include <chrono>
#include <fstream>

#include <FI_injector.h>

FI_injector::FI_injector()
{
    
}

FI_injector::FI_injector(char *processLocation)
{
    m_processLocation = processLocation;
    //m_threadID = -1;                // Will be used for random injection
    m_threadSize = 9;                // Will be used for random injection
}

FI_injector::FI_injector(char *processLocation, int threadID, FI_injector::intel_registers reg)
{
    m_processLocation = processLocation;
    m_threadID = threadID;
    m_register = reg;
}


FI_injector::~FI_injector()
{
    
}

bool FI_injector::run_injection()
{
    bool result;

    // Wait a random number of microseconds
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Start the process and wait for it to start up
    start_process();
    usleep(10000); // 1/100th of a second
    
    m_threadID = std::rand() % m_threadSize;

    // Attach to thread (9 in this case)
    attach_to_thread();
    
    // Generate a random number in the range of 0 to average process time
    int randomDelay = std::rand() % m_processTime;

    not_a_nice_way = randomDelay;

    usleep(randomDelay);

    // Perform injection
    result = inject_fault(get_random_register());

    // Kill parent process
    kill(m_process, SIGKILL);

    return result;
}

// Starts a process specified 
pid_t FI_injector::start_process()
{
    pid_t pid = fork();
    int status;
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

pid_t FI_injector::attach_to_thread()
{
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/task", m_process);
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return -1;
    }

    struct dirent *entry;
    int count = 0;
    pid_t tid = -1;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            if (count == m_threadID) {
                tid = std::stoi(entry->d_name);
                break;
            }
            count++;
        }
    }
    closedir(dir);

    if (tid == -1) {
        std::cerr << "Thread index " << m_threadID << " not found" << std::endl;
    }

    m_thread = tid;

    return tid;
}

int FI_injector::time_process(int iterations) {
    long long total_time = 0;

    for (int i = 0; i < iterations; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        
        pid_t pid = start_process();
        if (pid == -1) {
            std::cerr << "Failed to start the process" << std::endl;
            continue;
        }

        int status;
        waitpid(pid, &status, 0); // Wait for the process to finish

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); // Duration in microseconds
        total_time += duration.count();
    }

    m_processTime = static_cast<int>(total_time / iterations);

    return m_processTime; // Return the average time in microseconds
}

int FI_injector::get_thread_size()
{
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/status", m_processLocation);
    std::ifstream status_file(path);
    if (!status_file.is_open()) {
        perror("ifstream");
        m_threadSize = -1;
    }

    std::string line;
    while (std::getline(status_file, line)) {
        if (line.find("Threads:") == 0) {
            int num_threads = std::stoi(line.substr(8));
            status_file.close();
            m_threadSize = num_threads;
        }
    }

    status_file.close();
    std::cerr << "Failed to find the number of threads in the status file" << std::endl;
    m_threadSize = -1;

    printf("number of threads: %d \n", m_threadSize);
    return m_threadSize;
}

char* FI_injector::get_register(FI_injector::intel_registers reg)
{
    switch(reg) {
        case RAX: return "rax";
        case RBX: return "rbx";
        case RCX: return "rcx";
        case RDX: return "rdx";
        case RSI: return "rsi";
        case RDI: return "rdi";
        case RSP: return "rsp";
        case RBP: return "rbp";
        case R8: return "r8";
        case R9: return "r9";
        case R10: return "r10";
        case R11: return "r11";
        case R12: return "r12";
        case R13: return "r13";
        case R14: return "r14";
        case R15: return "r15";
        default: return "unknown";
    }
}

bool FI_injector::inject_fault(intel_registers reg)
{
    bool result;

    // Pick a random register
    if (reg == intel_registers::RANDOM) {
        reg = get_random_register();
    }

    // Attach to the process
    if (ptrace(PTRACE_ATTACH, m_thread, nullptr, nullptr) == -1) {
        std::cerr << "Failed to attach to process: " << strerror(errno) << std::endl;
        return false;
    }

    // Wait for the process to stop
    int status;
    waitpid(m_thread, &status, 0);

    if (!WIFSTOPPED(status)) {
        std::cerr << "Process did not stop as expected." << std::endl;
        ptrace(PTRACE_DETACH, m_thread, nullptr, nullptr);
        return false;
    }

    // Read the registers
    struct user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, m_thread, nullptr, &regs) == -1) {
        std::cerr << "Failed to get registers: " << strerror(errno) << std::endl;
        ptrace(PTRACE_DETACH, m_thread, nullptr, nullptr);
        return false;
    }

    // Perform the bit flip on the specified register
    flip_bit(reg, regs);

    // Write the modified registers back to the process
    if (ptrace(PTRACE_SETREGS, m_thread, nullptr, &regs) == -1) {
        std::cerr << "Failed to set registers: " << strerror(errno) << std::endl;
        ptrace(PTRACE_DETACH, m_thread, nullptr, nullptr);
        return false;
    }

    // Detach from the process
    if (ptrace(PTRACE_DETACH, m_thread, nullptr, nullptr) == -1) {
        std::cerr << "Failed to detach from process: " << strerror(errno) << std::endl;
        return false;
    }

    // Wait for the process to continue and check if it crashes
    sleep(1); // Give it some time to potentially crash

    if (kill(m_thread, 0) == -1 && errno == ESRCH) {
        // Process does not exist anymore, it must have crashed
        //printf("- process crashed -\n");
        result = true;
    } else {
        // Process is still running
        result = false;
    }

    return result;
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
    }

    printf("bitflip in thread %d \t register: %s \t after: %d microseconds\n", m_threadID, get_register(reg), not_a_nice_way);
}

FI_injector::intel_registers FI_injector::get_random_register()
{
    // Seed the random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Generate a random number in the range of enum values excluding RANDOM
    int randomValue = std::rand() % RANDOM;

    // Return the corresponding enum value
    return static_cast<intel_registers>(randomValue);    
}

void FI_injector::list_threads(pid_t pid) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/task", pid);

    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    printf("Threads of process %d:\n", pid);
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            // Check if the directory name is a number
            char *endptr;
            long tid = strtol(entry->d_name, &endptr, 10);
            if (*endptr == '\0') {
                printf("Thread ID: %ld\n", tid);
            }
        }
    }

    closedir(dir);
}