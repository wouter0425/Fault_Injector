#include <controller.h>

int main()
{
#if defined(NMR)
    Controller* controller = new Controller("targets/NMR", "NMR", 50, 1000, 1000, 10, false);
    controller->add_target("task_A_1");
    controller->add_target("task_B_1");
    controller->add_target("task_B_2");
    controller->add_target("task_B_3");
    controller->add_target("voter");
    controller->add_target("task_C_1");
    controller->init_controller(5, 1, {0,1,2,3});
    controller->run_injection();
#elif defined (RAVNMR)
    Controller* controller = new Controller("targets/RAV-NMR", "RAV-NMR", 50, 1000, 1000, 10, false);
    controller->add_target("task_A_1");
    controller->add_target("task_B_1");
    controller->add_target("task_B_2");
    controller->add_target("task_B_3");
    controller->add_target("voter");
    controller->add_target("task_C_1");
    controller->init_controller(5, 1, {0,1,2,3});
    controller->run_injection();
#else
    Controller* controller = new Controller("targets/baseline", "baseline", 50, 1000, 1000, 10, false);
    controller->add_target("task_A");
    controller->add_target("task_B");    
    controller->add_target("task_C");
    controller->init_controller(5, 1, {0,1,2,3});
    controller->run_injection();
#endif

}
