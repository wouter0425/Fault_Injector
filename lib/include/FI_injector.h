#ifndef FI_INJECTOR_H
#define FI_INJECTOR_H

#include <sys/types.h>

class FI_injector
{
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
        R15,
        RANDOM
    };

    private:
        char *m_processLocation;
        int m_threadID;
        FI_injector::intel_registers m_register;
        pid_t m_process;
        pid_t m_thread;
        
    public:

        FI_injector();
        FI_injector(char *processLocation); // SEU
        FI_injector(char *processLocation, int threadID, FI_injector::intel_registers reg); // Stuck bit
        ~FI_injector();


        //virtual bool run();

        bool run_injection();
        pid_t start_process();
        pid_t attach_to_thread();
        char* get_register(FI_injector::intel_registers reg);
        bool inject_fault(intel_registers reg = intel_registers::RANDOM);
        pid_t get_pid_by_name(const char* process_name);
        void list_threads(pid_t pid);
        FI_injector::intel_registers get_random_register();
        int get_random_thread();
        void flip_bit(FI_injector::intel_registers reg, struct user_regs_struct &regs);
};

#endif