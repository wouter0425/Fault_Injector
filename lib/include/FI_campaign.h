#ifndef FI_CAMPAIGN_H
#define FI_CAMPAIGN_H

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

class FI_campaign
{
    private:
        FI_injector* m_injector;
        FI_logger* m_logger;

        string m_outputDirectory {"results"};
        string m_targetLocation;
        //string m_name;
        //intel_registers m_register;        
        pid_t m_target; // stores the parent target process
        //string m_targetName;
        //pid_t m_process;

        int m_burstTime;
        int m_burstFrequency;

        int m_baseBurstTime;
        int m_baseBurstFrequency;

        int m_startupDelay;
        int m_injectionDelay;

        long m_startTime;
        chrono::steady_clock::time_point m_logTime;
        //vector<string> m_targets;
        //vector<FI_result*> m_results;
        vector<FI_target*> m_targets;
        
        int m_cores[NUM_OF_CORES] = TARGET_CORES;
        int m_target_cores[NUM_OF_TARGET_CORES];

    public:

        FI_campaign();
        FI_campaign(string targetLocation, string outputDirectory, int startupDelay, int burstTime, int burstFrequency, int injectionDelay);
        ~FI_campaign();

        long get_start_time() { return m_startTime; }
        void run_injection();

        bool get_target_PIDs();
        bool stop_targets();
        bool start_targets();

        void create_targets(const chrono::steady_clock::time_point& start_time);        

        void add_target(string name) { m_targets.push_back(FI_target::declare_target(name, 0, 0)); }

        void set_target_location(string targetLocation) { m_targetLocation = targetLocation; }
        string get_target_location() {return m_targetLocation; }
        void FI_pause() {this_thread::sleep_for(chrono::milliseconds(m_burstFrequency));}
        FI_logger* get_logger() { return  m_logger;} 

        void set_burst_time(int burstTime) 
        { 
            m_burstTime = burstTime;
            m_baseBurstTime = burstTime; 
        }

        int get_burst_time() { return m_baseBurstTime; }

        void set_burst_frequency(int burstFrequency)
        { 
            m_burstFrequency = burstFrequency;
            m_baseBurstFrequency = burstFrequency; 
        }

        const int (&get_cores() const)[NUM_OF_CORES] { return m_cores; }
        vector<FI_target*> get_targets() const { return m_targets; }

        int get_burst_frequency() { return m_baseBurstFrequency; }

        void set_startup_delay(int startupDelay) { m_startupDelay = startupDelay * MILISECOND; }
        int get_startup_delay() { return m_startupDelay; }
        void set_injection_delay(int injectionDelay) { m_injectionDelay = injectionDelay * MILISECOND; } 
        int get_injection_delay() { return m_injectionDelay; }

        void set_result_directory(string dir) { m_logger->set_result_directory(dir); }
        //void create_parameter_file(string &path);
        void cleanup_campaign();

        pid_t start_process();
        //int time_process(int iterations);
        intel_registers get_random_register();
        //void inject_fault();
        
        void flip_bit(intel_registers reg, struct user_regs_struct &regs);
        //bool get_random_child_pid();
        bool active();
        bool burst_active(const chrono::steady_clock::time_point& start_time);
        //void get_random_core();
        void get_random_cores();
        
        void apply_random_frequency_deviation();

        bool get_child_PIDs();
        int get_core_of_process(pid_t process);
        bool get_target_process();
        
        //void add_result(FI_result* result) { m_results.push_back(result); };
        //void write_results_to_file();        

        //vector<FI_result*>& getResults() { return m_results; }
        //void setResults(const vector<FI_result*>& results) { m_results = results; }

        bool get_random_PID(char* path);
        bool get_process_name();
};

#endif