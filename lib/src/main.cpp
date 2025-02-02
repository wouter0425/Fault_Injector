#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <FI_campaign.h>

void handle_signal(int sig);

FI_campaign* campaign_test;
// FI_campaign* campaign_1_golden_run_standard;
// FI_campaign* campaign_1_golden_run_weighted;
// FI_campaign* campaign_1_standard;
// FI_campaign* campaign_1_weighted;
// FI_campaign* campaign_2_standard;
// FI_campaign* campaign_2_weighted;
// FI_campaign* campaign_3_standard;
// FI_campaign* campaign_3_weighted;
// FI_campaign* campaign_4_standard;
// FI_campaign* campaign_4_weighted;
// FI_campaign* campaign_5_noNMR_0;
// FI_campaign* campaign_5_noNMR_1;
// FI_campaign* campaign_5_noNMR_2;
// FI_campaign* campaign_5_noNMR_3;
// FI_campaign* campaign_5_noNMR_4;


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

    campaign_test = new FI_campaign("FI_targets/main", "single_weighted_test", 1000, 1000, 1000, 5, false);
    campaign_test->add_target("task_A_1");
    campaign_test->add_target("task_B_1");
    campaign_test->add_target("task_B_2");
    campaign_test->add_target("task_B_3");
    campaign_test->add_target("voter");
    campaign_test->add_target("task_C_1");
    campaign_test->init_campaign(5, 1, {0});
    campaign_test->run_injection();

// proof_standard_injection
// proof_standard_targets

    // campaign_1_golden_run_standard = new FI_campaign("FI_targets/standard/main", "goldenrun_standard", 50, 1000, 1000, 10, true);
    // campaign_1_golden_run_standard->add_target("task_A_1");
    // campaign_1_golden_run_standard->add_target("task_B_1");
    // campaign_1_golden_run_standard->add_target("task_B_2");
    // campaign_1_golden_run_standard->add_target("task_B_3");
    // campaign_1_golden_run_standard->add_target("voter");
    // campaign_1_golden_run_standard->add_target("task_C_1");
    // campaign_1_golden_run_standard->init_campaign(5, 1, {0,1,2,3});
    // campaign_1_golden_run_standard->run_injection();

    // campaign_1_golden_run_weighted = new FI_campaign("FI_targets/weighted/main", "goldenrun_weighted", 50, 1000, 1000, 10, true);
    // campaign_1_golden_run_weighted->add_target("task_A_1");
    // campaign_1_golden_run_weighted->add_target("task_B_1");
    // campaign_1_golden_run_weighted->add_target("task_B_2");
    // campaign_1_golden_run_weighted->add_target("task_B_3");
    // campaign_1_golden_run_weighted->add_target("voter");
    // campaign_1_golden_run_weighted->add_target("task_C_1");
    // campaign_1_golden_run_weighted->init_campaign(5, 1, {0,1,2,3});
    // campaign_1_golden_run_weighted->run_injection();

    // campaign_1_standard = new FI_campaign("FI_targets/standard/main", "dual_standard", 50, 1000, 1000, 10, false);
    // campaign_1_standard->add_target("task_A_1");
    // campaign_1_standard->add_target("task_B_1");
    // campaign_1_standard->add_target("task_B_2");
    // campaign_1_standard->add_target("task_B_3");
    // campaign_1_standard->add_target("voter");
    // campaign_1_standard->add_target("task_C_1");
    // campaign_1_standard->init_campaign(5, 1, {0,1,2,3});
    // campaign_1_standard->run_injection();

    // campaign_1_weighted = new FI_campaign("FI_targets/weighted/main", "single_weighted", 50, 1000, 1000, 10, false);
    // campaign_1_weighted->add_target("task_A_1");
    // campaign_1_weighted->add_target("task_B_1");
    // campaign_1_weighted->add_target("task_B_2");
    // campaign_1_weighted->add_target("task_B_3");
    // campaign_1_weighted->add_target("voter");
    // campaign_1_weighted->add_target("task_C_1");
    // campaign_1_weighted->init_campaign(5, 1, {0,1,2,3});
    // campaign_1_weighted->run_injection();

    // campaign_2_standard = new FI_campaign("FI_targets/standard/main", "dual_standard", 50, 1000, 1000, 10, false);
    // campaign_2_standard->add_target("task_A_1");
    // campaign_2_standard->add_target("task_B_1");
    // campaign_2_standard->add_target("task_B_2");
    // campaign_2_standard->add_target("task_B_3");
    // campaign_2_standard->add_target("voter");
    // campaign_2_standard->add_target("task_C_1");
    // campaign_2_standard->init_campaign(5, 2, {0,1,2,3});
    // campaign_2_standard->run_injection();

    // campaign_2_weighted = new FI_campaign("FI_targets/weighted/main", "dual_weighted", 50, 1000, 1000, 10, false);
    // campaign_2_weighted->add_target("task_A_1");
    // campaign_2_weighted->add_target("task_B_1");
    // campaign_2_weighted->add_target("task_B_2");
    // campaign_2_weighted->add_target("task_B_3");
    // campaign_2_weighted->add_target("voter");
    // campaign_2_weighted->add_target("task_C_1");
    // campaign_2_weighted->init_campaign(5, 2, {0,1,2,3});
    // campaign_2_weighted->run_injection();

    // campaign_3_standard = new FI_campaign("FI_targets/standard/main", "triple_standard", 50, 1000, 1000, 10, false);
    // campaign_3_standard->add_target("task_A_1");
    // campaign_3_standard->add_target("task_B_1");
    // campaign_3_standard->add_target("task_B_2");
    // campaign_3_standard->add_target("task_B_3");
    // campaign_3_standard->add_target("voter");
    // campaign_3_standard->add_target("task_C_1");
    // campaign_3_standard->init_campaign(5, 3, {0,1,2,3});
    // campaign_3_standard->run_injection();

    // campaign_3_weighted = new FI_campaign("FI_targets/weighted/main", "triple_weighted", 50, 1000, 1000, 10, false);
    // campaign_3_weighted->add_target("task_A_1");
    // campaign_3_weighted->add_target("task_B_1");
    // campaign_3_weighted->add_target("task_B_2");
    // campaign_3_weighted->add_target("task_B_3");
    // campaign_3_weighted->add_target("voter");
    // campaign_3_weighted->add_target("task_C_1");
    // campaign_3_weighted->init_campaign(5, 3, {0,1,2,3});
    // campaign_3_weighted->run_injection();

    // campaign_4_standard = new FI_campaign("FI_targets/standard/main", "quad_standard", 50, 1000, 1000, 10, false);
    // campaign_4_standard->add_target("task_A_1");
    // campaign_4_standard->add_target("task_B_1");
    // campaign_4_standard->add_target("task_B_2");
    // campaign_4_standard->add_target("task_B_3");
    // campaign_4_standard->add_target("voter");
    // campaign_4_standard->add_target("task_C_1");
    // campaign_4_standard->init_campaign(5, 4, {0,1,2,3});
    // campaign_4_standard->run_injection();

    // campaign_4_weighted = new FI_campaign("FI_targets/weighted/main", "quad_weighted", 50, 1000, 1000, 10, false);
    // campaign_4_weighted->add_target("task_A_1");
    // campaign_4_weighted->add_target("task_B_1");
    // campaign_4_weighted->add_target("task_B_2");
    // campaign_4_weighted->add_target("task_B_3");
    // campaign_4_weighted->add_target("voter");
    // campaign_4_weighted->add_target("task_C_1");
    // campaign_4_weighted->init_campaign(5, 4, {0,1,2,3});
    // campaign_4_weighted->run_injection();

    // campaign_5_noNMR_0 = new FI_campaign("FI_targets/NoNMR/main", "golden_no_nmr", 50, 1000, 1000, 10, true);
    // campaign_5_noNMR_0->add_target("task_A");
    // campaign_5_noNMR_0->add_target("task_B");
    // campaign_5_noNMR_0->add_target("task_C");
    // campaign_5_noNMR_0->init_campaign(5, 1, {0,1,2,3});
    // campaign_5_noNMR_0->run_injection();

    // campaign_5_noNMR_1 = new FI_campaign("FI_targets/NoNMR/main", "single_no_nmr", 50, 1000, 1000, 10, false);
    // campaign_5_noNMR_1->add_target("task_A");
    // campaign_5_noNMR_1->add_target("task_B");
    // campaign_5_noNMR_1->add_target("task_C");
    // campaign_5_noNMR_1->init_campaign(5, 1, {0,1,2,3});
    // campaign_5_noNMR_1->run_injection();

    // campaign_5_noNMR_2 = new FI_campaign("FI_targets/NoNMR/main", "dual_no_nmr", 50, 1000, 1000, 10, false);
    // campaign_5_noNMR_2->add_target("task_A");
    // campaign_5_noNMR_2->add_target("task_B");
    // campaign_5_noNMR_2->add_target("task_C");
    // campaign_5_noNMR_2->init_campaign(5, 2, {0,1,2,3});
    // campaign_5_noNMR_2->run_injection();

    // campaign_5_noNMR_3 = new FI_campaign("FI_targets/NoNMR/main", "triple_no_nmr", 50, 1000, 1000, 10, false);
    // campaign_5_noNMR_3->add_target("task_A");
    // campaign_5_noNMR_3->add_target("task_B");
    // campaign_5_noNMR_3->add_target("task_C");
    // campaign_5_noNMR_3->init_campaign(5, 3, {0,1,2,3});
    // campaign_5_noNMR_3->run_injection();

    // campaign_5_noNMR_4 = new FI_campaign("FI_targets/NoNMR/main", "quad_no_nmr", 50, 1000, 1000, 10, false);
    // campaign_5_noNMR_4->add_target("task_A");
    // campaign_5_noNMR_4->add_target("task_B");
    // campaign_5_noNMR_4->add_target("task_C");
    // campaign_5_noNMR_4->init_campaign(5, 4, {0,1,2,3});
    // campaign_5_noNMR_4->run_injection();
}

void handle_signal(int sig) 
{    
    campaign_test->get_logger()->output_tsv(campaign_test);
    // campaign_1_golden_run_standard->get_logger()->output_tsv(campaign_1_golden_run_standard);
    // campaign_1_golden_run_weighted->get_logger()->output_tsv(campaign_1_golden_run_weighted);
    // campaign_1_standard->get_logger()->output_tsv(campaign_1_standard);
    // campaign_1_weighted->get_logger()->output_tsv(campaign_1_weighted);
    // campaign_2_standard->get_logger()->output_tsv(campaign_2_standard);
    // campaign_2_weighted->get_logger()->output_tsv(campaign_2_weighted);
    // campaign_3_standard->get_logger()->output_tsv(campaign_3_standard);
    // campaign_3_weighted->get_logger()->output_tsv(campaign_3_weighted);
    // campaign_4_standard->get_logger()->output_tsv(campaign_4_standard);
    // campaign_4_weighted->get_logger()->output_tsv(campaign_4_weighted);

    //campaign_5_noNMR_0->get_logger()->output_tsv(campaign_5_noNMR_0);
    //campaign_5_noNMR_1->get_logger()->output_tsv(campaign_5_noNMR_1);
    //campaign_5_noNMR_2->get_logger()->output_tsv(campaign_5_noNMR_2);
    //campaign_5_noNMR_3->get_logger()->output_tsv(campaign_5_noNMR_3);
    //campaign_5_noNMR_4->get_logger()->output_tsv(campaign_5_noNMR_4);

    if (sig == SIGINT) 
    {
        printf("Scheduler received SIGINT, shutting down...\n");
        exit(EXIT_SUCCESS);
    }
}

