#ifndef JOB_H
#define JOB_H

#include <string>

#include "defines.h"

using namespace std;

class Job {
private:
    string m_name;
    pid_t m_pid;
    int m_core;
    intel_registers m_register;
public:
    Job();
    Job(string name, pid_t pid, int core, intel_registers reg);
    ~Job();

    static Job* declare_job(string name, pid_t pid, int core, intel_registers reg);
    intel_registers getRegister() const { return m_register; }
    pid_t getPid() const { return m_pid; }
    string getName() const { return m_name; }
    int getCore() const { return m_core; }
};

#endif