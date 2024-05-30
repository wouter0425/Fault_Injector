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
}

FI_campaign::~FI_campaign()
{

}

vector<bool> FI_campaign::run_campaign()
{
    FI_injector injector(m_process);
    bool result;

    for(int i = 0; i < m_iterations; i++)
    {
        printf("run %d / %d. \n", i, m_iterations);
        
        result = injector.run_injection();

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