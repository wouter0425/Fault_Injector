#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <FI_campaign.h>

void handle_signal(int sig);

FI_campaign* campaign_standard;
FI_campaign* campaign_weighted;

int main()
{
    // Register the signal handler for SIGINT
    signal(SIGINT, handle_signal);

    // Specify the CPU core to run the scheduler on
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);    
    CPU_SET(5, &cpuset);

    if (sched_setaffinity(0, sizeof(cpuset), &cpuset) != 0) 
    {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }

    /* FI parameters:
     *      - target_location: location of the target executable
     *      - startup delay: number of ms the injector waits after starting the target
     *      - burst_time: number of ms the injector attacks the target
     *      - burst_frequency: a delay in microseconds between injections    
     */
    campaign_standard = new FI_campaign("FI_targets/standard/main", "new_triple_standard", 50, 1000, 1000, 10);
    campaign_standard->add_target("task_A_1");
    campaign_standard->add_target("task_B_1");
    campaign_standard->add_target("task_B_2");
    campaign_standard->add_target("task_B_3");
    campaign_standard->add_target("voter");
    campaign_standard->add_target("task_C_1");    
    campaign_standard->run_injection();    

    campaign_weighted = new FI_campaign("FI_targets/weighted/main", "new_triple_dual_weighted", 50, 1000, 1000, 10);
    campaign_weighted->add_target("task_A_1");
    campaign_weighted->add_target("task_B_1");
    campaign_weighted->add_target("task_B_2");
    campaign_weighted->add_target("task_B_3");
    campaign_weighted->add_target("voter");
    campaign_weighted->add_target("task_C_1");    
    campaign_weighted->run_injection();
}

void handle_signal(int sig) 
{
    //campaign_standard->get_logger()->output_tsv(campaign_standard);
    campaign_weighted->get_logger()->output_tsv(campaign_weighted);

    if (sig == SIGINT) 
    {
        printf("Scheduler received SIGINT, shutting down...\n");
        exit(EXIT_SUCCESS);
    }
}

