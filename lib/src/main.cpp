#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <FI_campaign.h>
#include <FI_injector.h>

int main()
{
    //FI_injector injector;
    // FI_injector injector("/home/wouter/Documents/main","main", 9);
    // int iterations = 10;
    // for(int i = 0; i <= iterations; i++)
    // {
    //     printf("run %d / %d. \n", i, iterations);

    //     injector.run_injection();
    // }

    FI_campaign campaign("/home/wouter/Documents/main", 10);

    campaign.run_campaign();

    campaign.print_campaign();
    


    //injector.get_thread_id();
    //injector.inject_fault(FI_injector::RAX);
}
