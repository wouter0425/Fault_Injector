#ifndef FI_DEFINES_H
#define FI_DEFINES_H

#define RANDOM_CORE false
#define NUM_OF_CORES    4

#define SECOND  1000000
#define MILISECOND 1000
#define BURST_TIME  1000
#define BASE_FREQ   1000

#define TARGET_CORES    {0,1,2,3}
#define NUM_OF_TARGETS  4
//#define GOLDEN_RUN

enum intel_registers {
    RAX,
    RBX,
    RCX,
    RDX,
    RSI,
    RDI,
    RSP,
    RBP,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
    RANDOM
};


#endif