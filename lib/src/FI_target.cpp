#include "FI_target.h"

FI_target::FI_target()
{

}

FI_target::FI_target(string name, int pid, int core)
{
    m_name = name;
    m_pid = pid;
    m_core = core;
}

FI_target::~FI_target()
{
    
}

FI_target* FI_target::declare_target(string name, int pid, int core)
{
    FI_target* t = new FI_target(name, pid, core);
    return t;
}