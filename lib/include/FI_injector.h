#ifndef FI_INJECTOR_H
#define FI_INJECTOR_H

// startup delay in micro seconds
#define STARTUP_DELAY   100000

#include <sys/types.h>
#include <vector>
#include <chrono>
#include <FI_defines.h>
#include <FI_result.h>

using namespace std;

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
        std::string m_name;
        int m_threadID;
        FI_injector::intel_registers m_register;
        pid_t m_process;
        int m_core {0}; // First run is never an error
        pid_t m_thread;
        int m_processTime;
        int m_threadSize;
        int m_injectionTime;
        int m_burstTime;
        vector<FI_result> m_results;
        int m_cores[NUM_OF_TARGETS] = TARGET_CORES;

    public:

        FI_injector();
        FI_injector(char *processLocation); // SEU
        FI_injector(char *processLocation, int threadID, FI_injector::intel_registers reg); // Stuck bit
        ~FI_injector();

        FI_result run_injection();
        pid_t start_process();
        int time_process(int iterations);
        int count_threads();
        pid_t attach_to_thread();
        //char* get_register(FI_injector::intel_registers reg);
        pid_t get_pid_by_name(const char* process_name);
        void list_threads(pid_t pid);
        void get_random_register();
        void inject_fault();
        int get_random_thread();
        void flip_bit(FI_injector::intel_registers reg, struct user_regs_struct &regs);
        char* get_process_name();
        bool get_random_child_pid();
        bool is_process_running();
        int get_core_of_child_process(pid_t child_pid);
        bool burst_active(const std::chrono::steady_clock::time_point& start_time);
        bool contains(int target);
        void get_random_core();

        std::vector<int> get_active_cores();
        void add_result(time_t t);
        void write_results_to_file();

};

#endif