#include <FI_logger.h>
#include <FI_campaign.h>

#include <FI_defines.h>

#include <sys/stat.h>
#include <cerrno>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <iomanip>

FI_logger::FI_logger()
{

}

FI_logger::FI_logger(string resultDirectory)
{
    m_resultDirectory = resultDirectory;
}

FI_logger::~FI_logger()
{
    m_results.clear();   
}

void FI_logger::output_tsv(FI_campaign* c)
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

    // Create column names for injection_file
    fprintf(injection_file, "time\t");
    fprintf(target_file, "time\t");

    for (int i = 0; i < NUM_OF_CORES; i++)
    {
        fprintf(injection_file, "Core %d", c->get_cores()[i]);
        if (i < NUM_OF_CORES - 1)
            fprintf(injection_file, "\t");
    }

    fprintf(injection_file, "\n");

    // create column names for target_file
    for (size_t i = 0; i < c->get_targets().size(); i++)
    {
        fprintf(target_file, "%s", c->get_targets()[i]->get_name().c_str());

        if (i < c->get_targets().size() - 1)
            fprintf(target_file, "\t");        
    }

    fprintf(target_file, "\n");

    // Declare the arrays to store the results
    int injection_sum[NUM_OF_CORES];
    int target_sum[c->get_targets().size()];

    // Initialize the array results
    for (size_t i = 0; i < NUM_OF_CORES; i++) injection_sum[i] = 0;
    for (size_t i = 0; i < c->get_targets().size(); i++) target_sum[i] = 0; 

    // Process the results
    for (auto& result: m_results)
    {

        for (size_t i = 0; i < NUM_OF_CORES; i++) 
        {
            if (result->targetCoreExists(c->get_cores()[i]))
            {
                injection_sum[i]++;
                //break;
            }
        }

        for (size_t i = 0; i < c->get_targets().size(); i++)
        {
            if (result->targetNameExists(c->get_targets()[i]->get_name()))
            {
                target_sum[i]++;
                //break;
            }         
        }

        fprintf(injection_file, "%ld\t", result->get_time());
        fprintf(target_file, "%ld\t", result->get_time());

        for (size_t i = 0; i < NUM_OF_CORES; i++)
        {
            fprintf(injection_file, "%d", injection_sum[i]);
            if (i < NUM_OF_CORES - 1)            
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

int FI_logger::create_directory(string &path) {
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

void FI_logger::create_parameter_file(string &path, FI_campaign* c)
{
    FILE *injection_file = fopen(path.c_str(), "w");

    if (!injection_file)
        return;

    fprintf(injection_file, "*** Parameter file *** \n");
    //fprintf(injection_file, "");
    fprintf(injection_file, "\t target name:     %s \n", c->get_target_location().c_str());
    fprintf(injection_file, "\t startup delay:   %d ms \n", c->get_startup_delay());
    fprintf(injection_file, "\t burst duration:  %d ms \n", c->get_burst_time());
    fprintf(injection_file, "\t burst frequency: %d ms \n", c->get_burst_frequency());
    fprintf(injection_file, "\t injection delay: %d ms \n", c->get_injection_delay() / MILISECOND);
    fprintf(injection_file, "\t number of cores: %d \n", NUM_OF_CORES);
    fprintf(injection_file, "\t Golden run:      %d (0 is false) \n", GOLDEN_RUN);
    
    fprintf(injection_file, "\t target cores:    %d \n", NUM_OF_TARGET_CORES);
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

string FI_logger::generate_output_string(const string& prefix) {
    // Get the current time
    std::time_t now = std::time(nullptr);
    std::tm timeinfo = *std::localtime(&now);

    // Format the date and time
    std::ostringstream oss;
    oss << std::put_time(&timeinfo, "%Y-%m-%d_%H-%M-%S");
    string suffix = oss.str();

    // Construct the output string
    string output = prefix + "_" + suffix;

    return output;
}