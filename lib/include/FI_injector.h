#ifndef FI_INJECTOR_H
#define FI_INJECTOR_H

#include <sys/types.h>

class FI_injector
{
    private:
        char *m_processName;
        int m_threadID;
        pid_t m_process;
        pid_t m_thread;
        
    public:
        FI_injector();
        FI_injector(char *processName, int threadID);
        ~FI_injector();

        enum intel_registers {
            RAX,
            RBX,
            RCX,
            RDX,
            RSI,
            RDI,
            RSP,
            RBP,
            R8,
            R9,
            R10,
            R11,
            R12,
            R13,
            R14,
            R15
        };

        pid_t start_process();
        pid_t get_thread_id();
        char* get_register(FI_injector::intel_registers reg);
        



    
};

#endif