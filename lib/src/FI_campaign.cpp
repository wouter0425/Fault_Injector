#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>

#include <FI_campaign.h>
#include <FI_injector.h>

FI_campaign::FI_campaign()
{

}

FI_campaign::FI_campaign(char *process, int iterations)
{
    m_processPath = process;
    m_iterations = iterations;
    m_failCounter = 0;
    m_errorCounter = 0;
    m_successCounter = 0;
}

FI_campaign::~FI_campaign()
{

}

void FI_campaign::run_campaign()
{
    FI_injector injector(m_processPath);

    printf("Timing process....\n");
    injector.time_process(1);

    printf("get process name...\n");
    m_processName = injector.get_process_name();

    for(int i = 1; i <= m_iterations; i++)
    {        
        FI_result result = injector.run_injection();

        printf("run %d / %d. \t return: %d \n", i, m_iterations, result.getResultType());

        switch(result.getResultType()) {
            case 0: m_successCounter++; break;
            case 1: m_failCounter++; break;
            case 3: m_errorCounter++; break;
            default: m_failCounter++; break;
        }

        m_results.push_back(result);
    }
}

void FI_campaign::print_campaign()
{
    // for(int i = 0; i < m_iterations; i++)
    // {
    //     printf("run %d result: %d \n", i, static_cast<int>(m_results[i]));
    // }

    return;
}

void FI_campaign::write_results_to_file()
{
    // Get the current time
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    // Create a time string
    std::ostringstream timeStream;
    timeStream << std::put_time(&tm, "%Y%m%d_%H%M%S");

    // Construct the directory path
    const char* directory = "FI_results/";
    
    // Ensure the directory exists
    struct stat info;
    if (stat(directory, &info) != 0) {
        // Directory does not exist, create it
        if (mkdir(directory, 0777) != 0) {
            std::cerr << "Error creating directory: " << directory << std::endl;
            return;
        }
    } else if (!(info.st_mode & S_IFDIR)) {
        // Directory path is not a directory
        std::cerr << "Path exists but is not a directory: " << directory << std::endl;
        return;
    }

    // Construct the file name
    std::string fileName = std::string(directory) + m_processName + "_" + timeStream.str() + ".txt";    

    // Open the file for writing
    std::ofstream outFile(fileName);
    if (!outFile) {
        std::cerr << "Error opening file for writing: " << fileName << std::endl;
        return;
    }

    outFile << "Iterations: " << m_iterations << "\n";
    outFile << "Passes: " << m_successCounter << "\n";
    outFile << "fails: " << m_failCounter << "\n";
    outFile << "errors: " << m_errorCounter << "\n\n";

    outFile << "<Result> \t <ResultType> \t <Time of injection> \t <Injected core> \t <Injected register> \n";

    // Write the results
    for (size_t i = 0; i < m_results.size(); ++i) {
        const FI_result& result = m_results[i];
        outFile << (i + 1) << "\t"
                << (result.getResult() ? "true" : "false") << "\t"
                << result.getResultType() << "\t"
                << result.getInjectionTime() << "\t"
                << result.getCore() << "\t"
                << result.getRegister() << "\n";
    }

    // Close the file
    outFile.close();
    if (!outFile) {
        std::cerr << "Error closing file: " << fileName << std::endl;
    }    
}