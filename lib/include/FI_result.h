#ifndef FI_RESULT_H
#define FI_RESULT_H

#include <vector>
#include <time.h>

class FI_result {

    private:
        std::vector<int> m_cores;
        int m_target;
        time_t m_time;

    public:
        FI_result(std::vector<int> cores, int target, time_t time);
        ~FI_result();

        std::vector<int> get_cores() const { return m_cores; }
        int get_target() const { return m_target; }
        time_t get_time() const { return m_time; }
};

#endif