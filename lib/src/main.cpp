#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <FI_campaign.h>
#include <FI_injector.h>

int main()
{
    FI_campaign campaign_1("/home/wouter/Development/Fault_Injector/FI_targets/main_ORG", 10);
    campaign_1.run_campaign();
    campaign_1.write_results_to_file();

    FI_campaign campaign_2("/home/wouter/Development/Fault_Injector/FI_targets/main_NMR", 10);
    campaign_2.run_campaign();
    campaign_2.write_results_to_file();
}
