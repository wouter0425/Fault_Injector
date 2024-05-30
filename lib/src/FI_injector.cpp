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


#include <FI_injector.h>

FI_injector::FI_injector()
{
    
}

FI_injector::FI_injector(char *processName, int threadID)
{
    m_processName = processName;
    m_threadID = threadID;
}

FI_injector::~FI_injector()
{
    
}

pid_t FI_injector::start_process()
{
    pid_t pid = fork();
    if (pid == 0) {
        execl(m_processName, m_processName, NULL);
        perror("execl");
        exit(1);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        return pid;
    } else {
        perror("fork");
        return -1;
    }
}

pid_t FI_injector::get_thread_id()
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
            if (count == m_thread) {
                tid = std::stoi(entry->d_name);
                break;
            }
            count++;
        }
    }
    closedir(dir);

    if (tid == -1) {
        std::cerr << "Thread index " << m_thread << " not found" << std::endl;
    }

    return tid;
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

