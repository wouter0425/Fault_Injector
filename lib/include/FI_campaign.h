#ifndef FI_CAMPAIGN_H
#define FI_CAMPAIGN_H

#include <vector>

using namespace std;

class FI_campaign {
    private:
        char *m_process;
        int m_iterations;
        vector<bool> m_results;
        int m_failCounter;
        int m_successCounter;

    public:
        FI_campaign();
        FI_campaign(char *process, int iterations);
        ~FI_campaign();

        //vector<bool> get_results() { return m_results; }
        vector<bool> run_campaign();
        void print_campaign();

};

#endif