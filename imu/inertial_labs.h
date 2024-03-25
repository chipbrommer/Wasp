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
    int ReadData() override;
    int SendData() override;

protected:

private:
    void ProcessData();
    void UpdateCommonData() override;

    InertialLabsData m_data = {};
};