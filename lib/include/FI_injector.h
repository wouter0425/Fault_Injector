#ifndef FI_INJECTOR_H
#define FI_INJECTOR_H

#include <sys/types.h>
#include <vector>

#include <FI_job.h>
#include <FI_defines.h>
#include <FI_result.h>
#include <FI_logger.h>

long current_time_in_ms();

class FI_injector {
private:
    vector<FI_job*> m_jobs;
    //vector<FI_result*> m_results;
    long m_startTime {0};

public:
    FI_injector() {};
    ~FI_injector() {};

    void add_job(FI_job* target);    
    void inject_faults(FI_logger* logger);
    void flip_bit(intel_registers reg, struct user_regs_struct &regs);
    int get_core_of_child_process();
    void clear_jobs() { m_jobs.clear(); }    
    void set_startTime(long startTime) { m_startTime = startTime; }    
};

#endif
