#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <sys/types.h>
#include <vector>
#include <chrono>
#include <string>
#include <thread>

#include <defines.h>
#include <result.h>
#include <injector.h>
#include <target.h>
#include <job.h>
#include <logger.h>

using namespace std;

long current_time_in_ms();
int create_directory(string &path);

using namespace std;

class Controller
{
    private:
        Injector* m_injector;
        Logger* m_logger;
        vector<Target*> m_targets;
        vector<int> m_cores;
        string m_outputDirectory {"results"};
        string m_targetLocation;
        pid_t m_target;
        int m_burstTime;
        int m_burstDelay;
        int m_baseBurstTime;
        int m_baseBurstDelay;
        int m_startupDelay;
        int m_injectionDelay;
        long m_startTime;
        chrono::steady_clock::time_point m_logTime;
        vector<int> m_target_cores;

    public:        
        Controller(string targetLocation, string outputDirectory, int startupDelay, int burstTime, int burstFrequency, int injectionDelay, bool goldenRun);
        ~Controller();

        static Controller* declare_controller(string targetLocation, string outputDirectory, int startupDelay, int burstTime, int burstFrequency, int injectionDelay, bool goldenRun);        
        void init_controller(int injectorCore, int numOfTargets, vector<int>targetCores);
        void run_injection();
        void cleanup_controller();
        bool get_target_PIDs();
        bool stop_targets();
        bool start_targets();
        void create_targets();
        void add_target(string name) { m_targets.push_back(Target::declare_target(name, 0, 0)); }        
        string get_target_location() {return m_targetLocation; }
        void pause() {this_thread::sleep_for(chrono::milliseconds(m_burstDelay));}
        Logger* get_logger() { return  m_logger;}
        int get_burst_time() { return m_baseBurstTime; }        
        vector<int> get_cores() const { return m_cores; }
        vector<int> get_target_cores() const { return m_target_cores; }
        vector<Target*> get_targets() const { return m_targets; }
        int get_burst_frequency() { return m_baseBurstDelay; }
        int get_startup_delay() { return m_startupDelay; }        
        int get_injection_delay() { return m_injectionDelay; }
        void set_result_directory(string dir) { m_logger->set_result_directory(dir); }
        pid_t start_process();
        intel_registers get_random_register();        
        void flip_bit(intel_registers reg, struct user_regs_struct &regs);
        bool active();
        bool burst_active(const chrono::steady_clock::time_point& start_time);
        void get_random_cores();        
        void apply_random_frequency_deviation();        
        int get_core_of_process(pid_t process);
};

#endif