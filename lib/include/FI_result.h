#ifndef FI_RESULT_H
#define FI_RESULT_H

#include <time.h>
#include <vector>
#include <string>

using namespace std;

typedef struct {
    string name;
    int injections;
} target;

class FI_result {

    private:        
        int m_target;
        string m_targetName;
        long m_time;
        vector<bool> m_targets;

    public:
        FI_result(int targetID, time_t time, vector<target> targetNames, string targetName);
        ~FI_result();
        
        int get_target() const { return m_target; }
        long get_time() const { return m_time; }
        vector<bool> get_targets() { return m_targets; }
        string get_target_name() const { return m_targetName; }
};

#endif