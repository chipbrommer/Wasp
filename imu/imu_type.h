#include <string>
#include "../utilities/serial_client.h"

struct ImuData
{
    double roll = 0.0;
    double pitch = 0.0;
    double yaw = 0.0;
    double rollDelta = 0.0;
    double pitchDelta = 0.0;
    double yawDelta = 0.0;

    bool hardwareError = false; // @todo fix these with some proper error types ??? 
    bool softwareError = false; // @todo fix these with some proper error types ??? 

    long txCount = 0;
    long txErrorCount = 0;
    long rxCount = 0;
    long rxErrorCount = 0;
};

class ImuType
{
public:
    ImuType();
    ImuType(std::string port, double baudrate);
    ~ImuType();
    bool Configure(std::string port, double baudrate);
    bool ReconfigureReconnect(std::string port, double baudrate);
    bool Connect();
    void Disconnect();
    ImuData GetCommonData();

protected:
    virtual void UpdateCommonData();

    ImuData m_commonData;

private:
    std::string m_port;
    double m_baudrate;

};