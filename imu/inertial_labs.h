#include "imu_type.h"

struct InertialLabsData
{
    double roll; 
    double pitch;
    double yaw;
    bool error;
};

class InertialLabs : ImuType
{
public:
    InertialLabs();
    ~InertialLabs();
    int ReadData();
    void ProcessData();
    int SendData();

protected:

private:
    void UpdateCommonData();

    InertialLabsData m_data;

    long m_txCount;
    long m_txErrorCount;
    long m_rxCount;
    long m_rxErrorCount;
};