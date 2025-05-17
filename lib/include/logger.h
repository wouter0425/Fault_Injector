#ifndef LOGGER
#define LOGGER

#include <vector>
#include <result.h>

class Controller;

class Logger {
private:
    vector<Result*> m_results;
    string m_resultDirectory {"results"};    

public:
    Logger();
    Logger(string resultDirectory);
    ~Logger();

    void cleanup_logger();
    void add_result(Result* result) { m_results.push_back(result); }
    void output_tsv(Controller* controller);
    string generate_output_string(const string& prefix);
    void create_parameter_file(string &path, Controller* controller);
    int create_directory(string &path);
    void set_result_directory(string dir) { m_resultDirectory = dir; } 
};

#endif