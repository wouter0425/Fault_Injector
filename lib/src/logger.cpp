#include <sys/stat.h>
#include <cerrno>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <iomanip>

#include <logger.h>
#include <controller.h>
#include <defines.h>

Logger::Logger()
{

}

Logger::Logger(string resultDirectory)
{
    m_resultDirectory = resultDirectory;
}

Logger::~Logger()
{
    m_results.clear();   
}

void Logger::cleanup_logger()
{
    for (Result* result : m_results)
    {
        delete result;
    }
}

void Logger::output_tsv(Controller* c)
{
    string directoryName = generate_output_string(m_resultDirectory);

    directoryName = "FI_results/" + directoryName;
    if (create_directory(directoryName))
        return;
    
    string injection_results = directoryName + "/injection.tsv";
    FILE *injection_file = fopen(injection_results.c_str(), "w");
    
    string target_results = directoryName + "/targets.tsv";
    FILE *target_file = fopen(target_results.c_str(), "w");

    if (!injection_file || !target_file)
    {
        perror("Failed to open file");
        return;
    }

    string parameterFile = directoryName + "/parameters.txt";
    create_parameter_file(parameterFile, c);

    fprintf(injection_file, "time\t");
    fprintf(target_file, "time\t");

    for (size_t i = 0; i < c->get_cores().size(); i++)
    {
        fprintf(injection_file, "Core %d", c->get_cores()[i]);
        if (i < c->get_cores().size() - 1)
            fprintf(injection_file, "\t");
    }

    fprintf(injection_file, "\n");

    for (size_t i = 0; i < c->get_targets().size(); i++)
    {
        fprintf(target_file, "%s", c->get_targets()[i]->get_name().c_str());

        if (i < c->get_targets().size() - 1)
            fprintf(target_file, "\t");        
    }

    fprintf(target_file, "\n");

    int injection_sum[c->get_cores().size()];
    int target_sum[c->get_targets().size()];

    for (size_t i = 0; i < c->get_cores().size(); i++) injection_sum[i] = 0;
    for (size_t i = 0; i < c->get_targets().size(); i++) target_sum[i] = 0; 

    for (auto& result: m_results)
    {

        for (size_t i = 0; i < c->get_cores().size(); i++) 
        {
            if (result->targetCoreExists(c->get_cores()[i]))
            {
                injection_sum[i]++;
            }
        }

        for (size_t i = 0; i < c->get_targets().size(); i++)
        {
            if (result->targetNameExists(c->get_targets()[i]->get_name()))
            {
                target_sum[i]++;
            }         
        }

        fprintf(injection_file, "%ld\t", result->get_time());
        fprintf(target_file, "%ld\t", result->get_time());

        for (size_t i = 0; i < c->get_cores().size(); i++)
        {
            fprintf(injection_file, "%d", injection_sum[i]);
            if (i < c->get_cores().size() - 1)            
                fprintf(injection_file, "\t");
            
        }

        for (size_t i = 0; i < c->get_targets().size(); i++)
        {
            fprintf(target_file, "%d", target_sum[i]);
            if (i < c->get_targets().size() - 1)
                fprintf(target_file, "\t");
        }

        fprintf(injection_file, "\n");
        fprintf(target_file, "\n");

    }

    fclose(injection_file);
    fclose(target_file);
}

int Logger::create_directory(string &path) 
{
    mode_t mode = 0755;
    if (mkdir(path.c_str(), mode) == -1) {
        if (errno == EEXIST) {
            std::cerr << "Directory already exists: " << path << std::endl;
        } else {
            perror("Error creating directory");
            return -1;
        }
    } else {
        std::cout << "Directory created: " << path << std::endl;
    }
    return 0;
}

void Logger::create_parameter_file(string &path, Controller* c)
{
    FILE *injection_file = fopen(path.c_str(), "w");

    if (!injection_file)
        return;

    fprintf(injection_file, "*** Parameter file *** \n");
    fprintf(injection_file, "\t target name:     %s \n", c->get_target_location().c_str());
    fprintf(injection_file, "\t startup delay:   %d ms \n", c->get_startup_delay());
    fprintf(injection_file, "\t burst duration:  %d ms \n", c->get_burst_time());
    fprintf(injection_file, "\t burst frequency: %d ms \n", c->get_burst_frequency());
    fprintf(injection_file, "\t injection delay: %d ms \n", c->get_injection_delay() / MILISECOND);
    fprintf(injection_file, "\t number of cores: %ld \n", c->get_cores().size());
    
    fprintf(injection_file, "\t target cores:    %ld \n", c->get_target_cores().size());
    for (int core : c->get_cores())
    {
        fprintf(injection_file, "\t\t core %d \n", core);
    }

    fprintf(injection_file, "\t targets: \n");
    for (auto*& target : c->get_targets())
    {
        fprintf(injection_file, "\t\t %s\n", target->get_name().c_str());
    }
}

string Logger::generate_output_string(const string& prefix)
{
    std::time_t now = std::time(nullptr);
    std::tm timeinfo = *std::localtime(&now);

    std::ostringstream oss;
    oss << std::put_time(&timeinfo, "%Y-%m-%d_%H-%M-%S");
    string suffix = oss.str();

    string output = prefix + "_" + suffix;

    return output;
}