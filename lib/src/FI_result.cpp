#include <FI_result.h>



FI_result::FI_result(std::vector<int> cores, int target, time_t time)
{
    m_cores = cores;
    m_target = target;
    m_time = time;
}

FI_result::~FI_result()
{

}
