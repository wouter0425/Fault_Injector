#include "injector.h"

#include <sys/ptrace.h>
#include <sys/user.h>
#include <sstream>
#include <iostream>
#include <cstdint>

void Injector::add_job(Job* job)
{
    m_jobs.push_back(job);
}

void Injector::inject_faults(Logger* logger)
{
    Result* result = new Result(current_time_in_ms() - m_startTime);

    for (Job* job : m_jobs)
    {
        struct user_regs_struct regs;
        if (ptrace(PTRACE_GETREGS, job->getPid(), nullptr, &regs) == -1) 
        {
            ptrace(PTRACE_DETACH, job->getPid(), nullptr, nullptr);
            continue;
        }
        
        flip_bit(job->getRegister(), regs);

        result->add_target_task(job->getName());
        result->add_target_core(job->getCore());        

        if (ptrace(PTRACE_SETREGS, job->getPid(), nullptr, &regs) == -1) 
        {
            ptrace(PTRACE_DETACH, job->getPid(), nullptr, nullptr);
            continue;
        }

        delete job;
    }

    logger->add_result(result);
}

void Injector::flip_bit(intel_registers reg, struct user_regs_struct &regs)
{
    if (m_goldenRun)
        return;

    int random_bit = std::rand() % 64;

    uint64_t mask = static_cast<uint64_t>(1) << random_bit;

    switch (reg) {
        case RAX: regs.rax ^= mask; break;
        case RBX: regs.rbx ^= mask; break;
        case RCX: regs.rcx ^= mask; break;
        case RDX: regs.rdx ^= mask; break;
        case RSI: regs.rsi ^= mask; break;
        case RDI: regs.rdi ^= mask; break;
        case RSP: regs.rsp ^= mask; break;
        case RBP: regs.rbp ^= mask; break;
        case R8:  regs.r8  ^= mask; break;
        case R9:  regs.r9  ^= mask; break;
        case R10: regs.r10 ^= mask; break;
        case R11: regs.r11 ^= mask; break;
        case R12: regs.r12 ^= mask; break;
        case R13: regs.r13 ^= mask; break;
        case R14: regs.r14 ^= mask; break;
        case R15: regs.r15 ^= mask; break;
        default:
            std::cerr << "Invalid register." << std::endl;
            return;
    }     
}

long current_time_in_ms() 
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    return (spec.tv_sec * 1000) + (spec.tv_nsec / 1000000);
}
