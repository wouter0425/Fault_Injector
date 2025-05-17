#include "target.h"

Target::Target()
{

}

Target::Target(string name, int pid, int core)
{
    m_name = name;
    m_pid = pid;
    m_core = core;
}

Target::~Target()
{
    
}

Target* Target::declare_target(string name, int pid, int core)
{
    Target* t = new Target(name, pid, core);
    return t;
}