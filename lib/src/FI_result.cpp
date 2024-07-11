#include <FI_result.h>



FI_result::FI_result(int targetID, time_t time, vector<target> targetNames, string targetName)
{    
    m_target = targetID;
    m_time = time;
    m_targetName = targetName;

    for (auto target : targetNames)
    {
        if (!target.name.compare(targetName))        
            m_targets.push_back(true);  
        else
            m_targets.push_back(false);
    }
}

FI_result::~FI_result()
{

}
