#ifndef FI_JOB_H
#define FI_JOB_H

#include <string>

#include "FI_defines.h"

using namespace std;

class FI_job {
private:
    string m_name;
    pid_t m_pid;
    int m_core;
    intel_registers m_register;
public:
    FI_job();
    FI_job(string name, pid_t pid, int core, intel_registers reg);
    ~FI_job();

    static FI_job* declare_job(string name, pid_t pid, int core, intel_registers reg);

    intel_registers getRegister() const { return m_register; }
    pid_t getPid() const { return m_pid; }
    string getName() const { return m_name; }
    int getCore() const { return m_core; }
};

#endif