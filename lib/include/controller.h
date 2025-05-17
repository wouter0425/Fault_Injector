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

        /**
         * @brief Declares a new Controller instance with specified parameters.
         *
         * This function allocates and initializes a new Controller instance. The 
         * Controller manages the fault injection process, including setting up target 
         * locations, logging output, and configuring timing parameters.
         *
         * @param targetLocation The target location for fault injection.
         * @param outputDirectory Directory for logging output data.
         * @param startupDelay Delay in microseconds before the injection process starts.
         * @param burstTime Duration of each fault burst in microseconds.
         * @param burstFrequency Frequency of fault bursts in Hz.
         * @param injectionDelay Delay in microseconds between individual injections.
         * @param goldenRun Boolean indicating if this is a golden run (no faults).
         * @return Pointer to the newly created Controller instance.
         */
        static Controller* declare_controller(string targetLocation, string outputDirectory, int startupDelay, int burstTime, int burstFrequency, int injectionDelay, bool goldenRun);        
        
        /**
         * @brief Adds a target to the Controller.
         *
         * This function appends a new target to the list of targets managed by the 
         * Controller. The target is initialized with default parameters.
         *
         * @param name The name of the target.
         */
        void add_target(string name) { m_targets.push_back(Target::declare_target(name, 0, 0)); } 

        /**
         * @brief Initializes the Controller with core configuration and target setup.
         *
         * This function sets the CPU affinity for the fault injector and assigns 
         * target cores for fault injection. The target cores are specified in the 
         * provided vector, and the injector core is set based on the parameter.
         *
         * @param injectorCore The core assigned to the fault injector.
         * @param numOfTargets The number of target cores.
         * @param targetCores Vector containing the IDs of the target cores.
         */
        void init_controller(int injectorCore, int numOfTargets, vector<int>targetCores);
        
        /**
         * @brief Executes the fault injection process.
         *
         * This function manages the complete fault injection process, including 
         * process startup, burst scheduling, fault injection, and logging. The 
         * injection process continues until the target is exits.
         */
        void run_injection();

        // Non-user required functions        
        void cleanup_controller();
        bool get_target_PIDs();
        bool stop_targets();
        bool start_targets();
        void create_targets();
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