#include <iostream>
#include <FI_campaign.h>
#include <FI_injector.h>

FI_campaign::FI_campaign()
{

}

FI_campaign::FI_campaign(char *process, int iterations)
{
    m_process = process;
    m_iterations = iterations;
    m_failCounter = 0;
    m_successCounter = 0;
}

FI_campaign::~FI_campaign()
{

}

vector<bool> FI_campaign::run_campaign()
{
    bool result;
    FI_injector injector(m_process);

    //injector.get_thread_size();    

    printf("Timing process....\n");
    injector.time_process(5);

    for(int i = 0; i < m_iterations; i++)
    {
        printf("run %d / %d. \n", i, m_iterations);
        
        result = injector.run_injection();

        result ? m_successCounter++ : m_failCounter++;

        m_results.push_back(result);
    }

    return m_results;
}

void FI_campaign::print_campaign()
{
    for(int i = 0; i < m_iterations; i++)
    {
        printf("run %d result: %d \n", i, static_cast<int>(m_results[i]));
    }

    return;
}