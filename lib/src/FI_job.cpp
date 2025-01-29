#include "FI_job.h"

#include<string.h>

FI_job::FI_job()
{

}

FI_job::FI_job(string name, pid_t pid, int core, intel_registers reg)
{    
    m_name = strdup(name.c_str());    
    m_pid = pid;
    m_core = core;
    m_register = reg;
}

FI_job::~FI_job()
{

}

FI_job* FI_job::declare_job(string name, pid_t pid, int core, intel_registers reg)
{
    FI_job* job = new FI_job(strdup(name.c_str()), pid, core, reg);
    return job;
}