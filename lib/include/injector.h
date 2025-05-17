#ifndef INJECTOR_H
#define INJECTOR_H

#include <sys/types.h>
#include <vector>

#include <job.h>
#include <defines.h>
#include <result.h>
#include <logger.h>

long current_time_in_ms();

class Injector {
private:
    vector<Job*> m_jobs;
    vector<Result*> m_resultsBuffer;
    long m_startTime {0};
    bool m_goldenRun { false };

public:
    Injector(bool goldenRun) { m_goldenRun = goldenRun; };
    ~Injector() {};

    void add_job(Job* target);    
    void inject_faults(Logger* logger);
    void flip_bit(intel_registers reg, struct user_regs_struct &regs);
    int get_core_of_child_process();
    void clear_jobs() { m_jobs.clear(); }    
    void set_startTime(long startTime) { m_startTime = startTime; }    
    bool is_golden_run() { return m_goldenRun; }
};

#endif
