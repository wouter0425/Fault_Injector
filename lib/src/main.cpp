#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <FI_injector.h>

void handle_signal(int sig);

FI_injector campaign_1;
FI_injector campaign_2;

int main()
{
    /* FI parameters:
     *      - target_location: location of the target executable
     *      - startup delay: number of ms the injector waits after starting the target
     *      - burst_time: number of ms the injector attacks the target
     *      - burst_frequency: a delay in microseconds between injections    
     */    
    /*
    campaign_1.set_target_location((char *)"FI_targets/ORG/main");
    campaign_1.set_startup_delay(100);
    campaign_1.set_burst_time(500);
    campaign_1.set_burst_frequency(1000000);
    campaign_1.add_target("task_A");
    campaign_1.add_target("task_B");
    campaign_1.add_target("task_C");
    campaign_1.run_injection();
    campaign_1.write_results_to_file();

    campaign_2.set_target_location((char *)"FI_targets/NMR/main");
    campaign_2.set_startup_delay(100);
    campaign_2.set_burst_time(500);
    campaign_2.set_burst_frequency(1000000);
    campaign_2.add_target("task_A_1");
    campaign_2.add_target("task_B_1");
    campaign_2.add_target("task_B_2");
    campaign_2.add_target("task_B_3");
    campaign_2.add_target("voter");
    campaign_2.add_target("task_C_1");
    campaign_2.run_injection();
    campaign_2.write_results_to_file();
    
    campaign_1.print_results();
    campaign_2.print_results(); 
    */

    // campaign_2.set_target_location((char *)"FI_targets/main");
    // campaign_2.set_startup_delay(100);
    // campaign_2.set_burst_time(100);
    // campaign_2.set_burst_frequency(1000000);
    // campaign_2.add_target("task_A");
    // campaign_2.add_target("task_B");
    // campaign_2.add_target("task_C");
    // campaign_2.run_injection();
    // campaign_2.write_results_to_file();

    // campaign_1.set_target_location((char *)"FI_targets/standard/main");
    // campaign_1.set_startup_delay(100);
    // campaign_1.set_burst_time(1000);
    // campaign_1.set_burst_frequency(1000);
    // campaign_1.add_target("task_A_1");
    // campaign_1.add_target("task_B_1");
    // campaign_1.add_target("task_B_2");
    // campaign_1.add_target("task_B_3");
    // campaign_1.add_target("voter");
    // campaign_1.add_target("task_C_1");
    // campaign_1.run_injection();
    // campaign_1.write_results_to_file();

    // campaign_2.set_target_location((char *)"FI_targets/weighted/main");
    // campaign_2.set_startup_delay(100);
    // campaign_2.set_burst_time(1000);
    // campaign_2.set_burst_frequency(1000);
    // campaign_2.add_target("task_A_1");
    // campaign_2.add_target("task_B_1");
    // campaign_2.add_target("task_B_2");
    // campaign_2.add_target("task_B_3");
    // campaign_2.add_target("voter");
    // campaign_2.add_target("task_C_1");
    // campaign_2.run_injection();
    // campaign_2.write_results_to_file();

    campaign_2.set_target_location((char *)"FI_targets/main");
    campaign_2.set_startup_delay(250);
    campaign_2.set_burst_time(1000);
    campaign_2.set_burst_frequency(1000);
    campaign_2.add_target("task_A_1");
    campaign_2.add_target("task_B_1");
    campaign_2.add_target("task_B_2");
    campaign_2.add_target("task_B_3");
    campaign_2.add_target("voter");
    campaign_2.add_target("task_C_1");
    campaign_2.run_injection();
    campaign_2.write_results_to_file();
}

void handle_signal(int sig) {
    
    campaign_1.print_results();    
    campaign_1.write_results_to_file();

    campaign_2.print_results();
    campaign_2.write_results_to_file();

    if (sig == SIGINT) 
    {
        printf("Scheduler received SIGINT, shutting down...\n");
        exit(EXIT_SUCCESS);
    }
}

