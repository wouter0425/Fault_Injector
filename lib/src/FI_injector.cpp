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
#include <FI_injector.h>
#include <FI_result.h>

FI_injector::FI_injector()
{

}

FI_injector::FI_injector(char *processLocation)
{
    m_processLocation = processLocation;
    m_threadID = -1;                // Will be used for random injection
    //m_threadSize = 9;                // Will be used for random injection
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

FI_result FI_injector::run_injection()
{
    int retVal;

    // Wait a random number of microseconds
    //std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::srand(time(0));

    // Start the process and wait for it to start up
    start_process();
    usleep(STARTUP_DELAY); // 1/10th of a second

    //m_threadID = (std::rand() % count_threads()) + 1;
    get_random_child_pid();

    // Attach to thread (9 in this case)
    attach_to_thread();

    // Generate a random number in the range of 0 to average process time
    m_injectionTime = std::rand() % (m_processTime - STARTUP_DELAY);

    // Wait before injecting
    usleep(m_injectionTime);

    //FI_injector::intel_registers reg = get_random_register();

    // Perform injection
    //retVal = inject_fault(reg);

    // Kill parent process
    kill(m_process, SIGKILL);

    // Create the result value
    //FI_result result(bool(retVal), retVal, m_injectionTime, m_threadID, get_register(reg));
    FI_result result;

    return result;
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

void FI_injector::inject_fault()
{
        // Read the registers
    struct user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, m_thread, nullptr, &regs) == -1) {
        //fprintf(stderr, "Failed to get registers: %s\n", strerror(errno));
        ptrace(PTRACE_DETACH, m_thread, nullptr, nullptr);
        return;
    }

    // Perform the bit flip on the specified register
    flip_bit(m_register, regs);

    printf("Injected in: %s \t pid: %d \t register: %s\n", m_name.c_str(), m_thread, get_register(m_register));

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

//FI_injector::intel_registers FI_injector::get_random_register()
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
        //printf("pid: %d \n", pid);
        child_pids.push_back(pid);
    }
    //printf("\n");
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
    m_thread = random_child_pid;

    // Construct the path to the comm file
    snprintf(path, sizeof(path), "/proc/%d/comm", m_thread);

    // Open the comm file to get the process name
    std::ifstream comm_file(path);
    if (comm_file) {
        std::getline(comm_file, m_name);
        comm_file.close();
    } else {
        perror("Failed to open comm file");
        return false;
    }

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
