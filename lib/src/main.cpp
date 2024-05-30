#include <iostream>

#include <FI_injector.h>

int main()
{
    //FI_injector injector;
    FI_injector injector("/home/wouter/Documents/main", 0);
    injector.start_process();
}