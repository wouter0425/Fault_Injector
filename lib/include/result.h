#ifndef Result_H
#define Result_H

#include <time.h>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Result {
    private:        
        long m_time;
        vector<string> m_target_tasks;
        vector<int> m_target_cores;

    public:              
        Result(time_t time);
        ~Result();
        
        long get_time() const { return m_time; }
        void setTime(long time) { m_time = time; }
        void add_target_task(string t) { m_target_tasks.push_back(t); }
        void add_target_core(int c) { m_target_cores.push_back(c); }
        const vector<string>& getTargets() const { return m_target_tasks; }
        const vector<int>& getTargetCores() const { return m_target_cores; }        
        bool targetNameExists(const string& targetName) const { return find(m_target_tasks.begin(), m_target_tasks.end(), targetName) != m_target_tasks.end(); }
        bool targetCoreExists(int targetCore) const {return find(m_target_cores.begin(), m_target_cores.end(), targetCore) != m_target_cores.end(); }
};

#endif