#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <chrono>

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
    m_outputFile = "output.txt";

}

FI_campaign::~FI_campaign()
{

}

void FI_campaign::run_campaign() {
    FI_injector injector(m_processPath);

    // Seed the random number generator
    std::srand(std::time(nullptr));

    injector.start_process();

    usleep(STARTUP_DELAY); // 1/10th of a second

    // Record the start time before entering the outer while loop
    auto overall_start_time = std::chrono::steady_clock::now();

    while (injector.is_process_running()) {
        for (int i = 0; i < 10; ++i) {
            // Frequency of the fault injection
            usleep(1000);

            // Calculate elapsed time since the overall start time
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - overall_start_time).count();

            // Log the active cores and pass the elapsed time
            injector.add_result(elapsed_time);
        }

        // Set the maximum amount of time for the inner while loop to run
        auto burst_start_time = std::chrono::steady_clock::now();

        while (injector.burst_active(burst_start_time)) {
            // Get a random intel register
            injector.get_random_register();

            // Get a random process
            if (injector.get_random_child_pid()) {
                injector.inject_fault();
            }

            usleep(10); // prevent busy loop

                        // Calculate elapsed time since the overall start time
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - overall_start_time).count();

            // Log the active cores and pass the elapsed time
            injector.add_result(elapsed_time);
        }
    }

    injector.write_results_to_file();
}
// void FI_campaign::write_results_to_file()
// {
//     std::ofstream file(m_outputFile, std::ios::out | std::ios::trunc);
//     if (!file) {
//         std::cerr << "Failed to open or create file for writing: " << m_outputFile << std::endl;
//         return;
//     }

//     // Write header
//     file << "time\ttarget core";
//     for (int i = 0; i < NUM_OF_CORES; ++i) {
//         file << "\tcore " << i;
//     }
//     file << "\n";

//     // std::vector<FI_result> get_results() { return m_results; }

//     // Write results
//     for (const auto& result : m_results) {
//         file << result.get_time() << "\t" << result.get_target();

//         std::vector<int> core_counters(NUM_OF_CORES, 0);
//         for (int core : result.get_cores()) {
//             if (core >= 0 && core < NUM_OF_CORES) {
//                 core_counters[core]++;
//             }
//         }

//         for (int counter : core_counters) {
//             file << "\t" << counter;
//         }
//         file << "\n";
//     }

//     file.close();
// }