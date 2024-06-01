#ifndef FI_CAMPAIGN_H
#define FI_CAMPAIGN_H

#include <vector>
#include <FI_result.h>

using namespace std;

class FI_campaign {
    private:
        char *m_processPath;
        char *m_processName;
        char *m_outputFile;
        int m_iterations;
        vector<FI_result> m_results;
        int m_failCounter;
        int m_errorCounter;
        int m_successCounter;

    public:
        FI_campaign();
        FI_campaign(char *process, int iterations);
        ~FI_campaign();

        //vector<bool> get_results() { return m_results; }
        void run_campaign();
        void print_campaign();
        void write_results_to_file();

};

#endif