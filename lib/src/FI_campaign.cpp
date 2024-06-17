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
}

FI_campaign::~FI_campaign()
{

}

void FI_campaign::run_campaign()
{
    FI_injector injector(m_processPath);

    // Seed the random number generator
    std::srand(time(NULL));

    injector.start_process();


    usleep(STARTUP_DELAY); // 1/10th of a second

    while(injector.is_process_running())
    {
        // frequency of the fault injection
        usleep(100000);

        // Get a random intel register
        injector.get_random_register();

        // Get a random process
        if (injector.get_random_child_pid())
        {
            injector.inject_fault();
        }
    }
}
