#ifndef FI_LOGGER
#define FI_LOGGER

#include <vector>
#include <FI_result.h>

class FI_controller;

class FI_logger {
private:
    vector<FI_result*> m_results;

    string m_resultDirectory {"results"};    

public:
    FI_logger();
    FI_logger(string resultDirectory);
    ~FI_logger();

    void cleanup_logger();

    void add_result(FI_result* result) { m_results.push_back(result); }

    void output_tsv(FI_controller* controller);

    string generate_output_string(const string& prefix);
    void create_parameter_file(string &path, FI_controller* controller);
    int create_directory(string &path);

    void set_result_directory(string dir) { m_resultDirectory = dir; } 
};

#endif