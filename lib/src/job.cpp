#include "job.h"

#include<string.h>

Job::Job()
{

}

Job::Job(string name, pid_t pid, int core, intel_registers reg)
{    
    m_name = strdup(name.c_str());    
    m_pid = pid;
    m_core = core;
    m_register = reg;
}

Job::~Job()
{

}

Job* Job::declare_job(string name, pid_t pid, int core, intel_registers reg)
{
    Job* job = new Job(strdup(name.c_str()), pid, core, reg);
    return job;
}