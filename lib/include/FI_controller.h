#ifndef FI_CONTROLLER_H
#define FI_CONTROLLER_H

#include <sys/types.h>
#include <vector>
#include <chrono>
#include <string>
#include <thread>

#include <FI_defines.h>
#include <FI_result.h>
#include <FI_injector.h>
#include <FI_target.h>
#include <FI_job.h>
#include <FI_logger.h>

using namespace std;

long current_time_in_ms();
string generateOutputString(const string& prefix);
int create_directory(string &path);


using namespace std;

class FI_controller
{
    private:
        FI_injector* m_injector;
        FI_logger* m_logger;

        string m_outputDirectory {"results"};
        string m_targetLocation;
        pid_t m_target;

        int m_burstTime;
        int m_burstFrequency;

        int m_baseBurstTime;
        int m_baseBurstFrequency;

        int m_startupDelay;
        int m_injectionDelay;

        long m_startTime;
        chrono::steady_clock::time_point m_logTime;
        vector<FI_target*> m_targets;

        vector<int> m_cores;
        vector<int> m_target_cores;

    public:        
        FI_controller(string targetLocation, string outputDirectory, int startupDelay, int burstTime, int burstFrequency, int injectionDelay, bool goldenRun);
        ~FI_controller();

        static FI_controller* declare_controller(string targetLocation, string outputDirectory, int startupDelay, int burstTime, int burstFrequency, int injectionDelay, bool goldenRun);

        long get_start_time() { return m_startTime; }
        void init_controller(int injectorCore, int numOfTargets, vector<int>targetCores);
        void run_injection();
        void cleanup_controller();

        bool get_target_PIDs();
        bool stop_targets();
        bool start_targets();

        void create_targets(const chrono::steady_clock::time_point& start_time);        

        void add_target(string name) { m_targets.push_back(FI_target::declare_target(name, 0, 0)); }

        void set_target_location(string targetLocation) { m_targetLocation = targetLocation; }
        string get_target_location() {return m_targetLocation; }
        void FI_pause() {this_thread::sleep_for(chrono::milliseconds(m_burstFrequency));}
        FI_logger* get_logger() { return  m_logger;} 

        void set_burst_time(int burstTime) { m_burstTime = burstTime; m_baseBurstTime = burstTime; }

        int get_burst_time() { return m_baseBurstTime; }

        void set_burst_frequency(int burstFrequency) { m_burstFrequency = burstFrequency; m_baseBurstFrequency = burstFrequency; }
        
        vector<int> get_cores() const { return m_cores; }
        vector<int> get_target_cores() const { return m_target_cores; }
        vector<FI_target*> get_targets() const { return m_targets; }

        int get_burst_frequency() { return m_baseBurstFrequency; }

        void set_startup_delay(int startupDelay) { m_startupDelay = startupDelay * MILISECOND; }
        int get_startup_delay() { return m_startupDelay; }
        void set_injection_delay(int injectionDelay) { m_injectionDelay = injectionDelay * MILISECOND; } 
        int get_injection_delay() { return m_injectionDelay; }

        void set_result_directory(string dir) { m_logger->set_result_directory(dir); }

        pid_t start_process();
        intel_registers get_random_register();
        
        void flip_bit(intel_registers reg, struct user_regs_struct &regs);
        bool active();
        bool burst_active(const chrono::steady_clock::time_point& start_time);
        void get_random_cores();
        
        void apply_random_frequency_deviation();

        bool get_child_PIDs();
        int get_core_of_process(pid_t process);
        bool get_target_process();        

        bool get_random_PID(char* path);
        bool get_process_name();
};

#endif