#include <FI_result.h>

FI_result::FI_result(bool result, int resultType, int injectionTime, int core, char* reg)
{
    m_result = result;
    m_resultType = resultType;
    m_injectionTime = injectionTime;
    m_core = core;
    m_register = reg;
}

FI_result::FI_result()
{

}

FI_result::~FI_result()
{

}
