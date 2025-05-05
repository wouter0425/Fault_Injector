#include <iostream>
#include <unistd.h>
#include <signal.h>

#include <FI_controller.h>

void handle_signal(int sig);

FI_controller* controller;

int main()
{
    // Register the signal handler for SIGINT
    signal(SIGINT, handle_signal);

    /* FI parameters:
     *      - target_location: location of the target executable
     *      - startup delay: number of ms the injector waits after starting the target
     *      - burst_time: number of ms the injector attacks the target
     *      - burst_frequency: a delay in microseconds between injections    
     */

    controller = new FI_controller("FI_targets/main/RAV-NMR", "RAV-NMR", 5000, 1000, 1000, 10, false);
    controller->add_target("task_A_1");
    controller->add_target("task_B_1");
    controller->add_target("task_B_2");
    controller->add_target("task_B_3");
    controller->add_target("voter");
    controller->add_target("task_C_1");
    controller->init_controller(5, 1, {0,1,2,3});
    controller->run_injection();
}

void handle_signal(int sig) 
{    
    controller->get_logger()->output_tsv(controller);

    if (sig == SIGINT) 
    {
        printf("Scheduler received SIGINT, shutting down...\n");
        exit(EXIT_SUCCESS);
    }
}

