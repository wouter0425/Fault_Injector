#ifndef TARGET_H
#define TARGET_H

#include <string>

using namespace std;

class Target {

private:
    string m_name;
    pid_t m_pid;
    int m_core;

public:
    Target();
    Target(string name, int pid, int core);
    ~Target();

    static Target* declare_target(string name, int pid, int core);
    string get_name() const { return m_name; }
    pid_t get_pid() const { return m_pid; }
    int get_core() const { return m_core; }
    void set_name(const string &name) { m_name = name; }
    void set_pid(pid_t pid) { m_pid = pid; }
    void set_core(int core) { m_core = core; }
};

#endif