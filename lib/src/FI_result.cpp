#include <FI_result.h>



FI_result::FI_result(time_t time)
{
    m_time = time;
}

// FI_result::FI_result(int targetID, time_t time, vector<target> targetNames, string targetName)
// {    
//     m_target = targetID;
//     m_time = time;
//     m_targetName = targetName;
// }

FI_result::~FI_result()
{

}
