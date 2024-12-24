#ifndef FI_INJECTOR_H
#define FI_INJECTOR_H

// startup delay in micro seconds
#define STARTUP_DELAY   100000

#include <sys/types.h>
#include <vector>
#include <chrono>
#include <string>
#include <FI_defines.h>
#include <FI_result.h>

using namespace std;

long current_time_in_ms();
string generateOutputString(const string& prefix);

using namespace std;

class FI_injector
{
    private:
        char *m_targetLocation;
        string m_name;
        intel_registers m_register;        
        int m_core {0}; // First run is never an error        
        pid_t m_target;
        string m_targetName;
        pid_t m_process;
        int m_burstTime;
        int m_burstFrequency;
        int m_startupDelay;
        long m_startTime;        
        vector<target> m_targets;
        vector<int> m_targetResults;
        vector<FI_result*> m_results;
        vector<pid_t> m_childPIDs;
        
        int m_cores[NUM_OF_CORES] = TARGET_CORES;

    public:

        FI_injector();
        FI_injector(char *processLocation, int startupDelay, int burstTime, int burstFrequency);        
        ~FI_injector();

        long get_start_time() { return m_startTime; }
        void run_injection();

        void add_target(string name) { m_targets.push_back({name, 0}); }
        void set_target_location(char *targetLocation) { m_targetLocation = targetLocation; }
        void set_burst_time(int burstTime) { m_burstTime = burstTime; }
        void set_burst_frequency(int burstFrequency) { m_burstFrequency = burstFrequency; }
        void set_startup_delay(int startupDelay) { m_startupDelay = startupDelay * MILISECOND; }

        pid_t start_process();
        int time_process(int iterations);
        
        void get_random_register();
        void inject_fault();
        
        void flip_bit(intel_registers reg, struct user_regs_struct &regs);
        bool get_random_child_pid();
        bool is_process_running();
        int get_core_of_child_process();
        bool burst_active(const std::chrono::steady_clock::time_point& start_time);
        void get_random_core();
        void print_results();
        void apply_random_frequency_deviation();

        bool get_child_PIDs();
        int get_core_of_process(pid_t process);
        bool get_target_process();
        
        void add_result(long time);
        void write_results_to_file();        

        vector<FI_result*>& getResults() { return m_results; }
        void setResults(const vector<FI_result*>& results) { m_results = results; }

        bool get_random_PID(char* path);
        bool get_process_name();
};

#endif