/////////////////////////////////////////////////////////////////////////////////
// @file            pwm_interface.cpp
// @brief           implementation of PWM class
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// 
// Includes:
//          name                    reason included
//          ------------------      ------------------------
#include    "pwm_interface.h"       // PWM header
//
/////////////////////////////////////////////////////////////////////////////////

PWM::PWMStatus PWM::ExportPWM()
{
    if (IsExported() == PWMStatus::Success) { return PWMStatus::Success; }

    std::ofstream file(m_pwmPath + "/export");

    if (!file) { return PWMStatus::Error; }

    file << m_pwmChannel;
    file.close();

    return PWMStatus::Success;
}

PWM::PWMStatus PWM::UnExportPWM()
{
    if (IsExported() == PWMStatus::NotExported) { return PWMStatus::Success; }

    std::ofstream file(m_pwmPath + "/unexport");

    if (!file) { return PWMStatus::Error; }

    file << m_pwmChannel;
    file.close();

    return PWMStatus::Success;
}

PWM::PWMStatus PWM::GetPeriod(size_t& period_ns)
{
    PWMStatus status = PWMStatus::Error;
    std::ifstream file(m_pwmPath + "/pwm" + std::to_string(m_pwmChannel) + "/period");

    if (!file) { return status; }

    std::string period;
    if (std::getline(file, period))
    {
        period_ns = std::stoi(period);
        status = PWMStatus::Success;
    }
    file.close();

    return status;
}

size_t PWM::GetPeriod()
{
    size_t period = -1;
    if (GetPeriod(period) == PWMStatus::Error) return -1;
    else return period;
}

PWM::PWMStatus PWM::SetPeriod(size_t period_ns)
{
    std::ofstream file(m_pwmPath + "/pwm" + std::to_string(m_pwmChannel) + "/period");

    if (!file) { return PWMStatus::Error; }

    file << period_ns;
    file.close();

    return PWMStatus::Success;
}

PWM::PWMStatus PWM::GetDutyCycle(size_t& dutyCycle_ns)
{
    PWMStatus status = PWMStatus::Error;
    std::ifstream file(m_pwmPath + "/pwm" + std::to_string(m_pwmChannel) + "/duty_cycle");

    if (!file) { return status; }

    std::string duty;
    if (std::getline(file, duty))
    {
        dutyCycle_ns = std::stoi(duty);
        status = PWMStatus::Success;
    }
    file.close();

    return status;
}

size_t PWM::GetDutyCycle()
{
    size_t duty = -1;
    if (GetDutyCycle(duty) == PWMStatus::Error) return -1;
    else return duty;
}

PWM::PWMStatus PWM::SetDutyCycle(int dutyCycle_ns)
{
    std::ofstream file(m_pwmPath + "/pwm" + std::to_string(m_pwmChannel) + "/duty_cycle");

    if (!file) { return PWMStatus::Error; }

    file << dutyCycle_ns;
    file.close();

    return PWMStatus::Success;
}

PWM::PWMStatus PWM::GetPolarity(PWMPolarity& polarity)
{
    PWMStatus status = PWMStatus::Error;

    std::ifstream file(m_pwmPath + "/pwm" + std::to_string(m_pwmChannel) + "/polarity");

    if (!file) { return status; }

    std::string read;
    if (std::getline(file, read))
    {
        if (read == "normal") { polarity = PWMPolarity::Normal; status = PWMStatus::Success; }
        else if (read == "inverted") { polarity = PWMPolarity::Inverted; status = PWMStatus::Success; }
    }
    file.close();

    return status;
}

PWM::PWMPolarity PWM::GetPolarity()
{
    PWMPolarity polarity = PWMPolarity::Unknown;
    if (GetPolarity(polarity) == PWMStatus::Error) return PWMPolarity::Unknown;
    else return polarity;
}

PWM::PWMStatus PWM::SetPolarity(const PWM::PWMPolarity polarity)
{
    if (DisablePWM() != PWMStatus::Success) { return PWMStatus::Error; }
    if (polarity == PWMPolarity::Unknown) { return PWMStatus::InvalidInput; }

    std::ofstream file(m_pwmPath + "/pwm" + std::to_string(m_pwmChannel) + "/polarity");

    if (!file) { return PWMStatus::Error; }

    if (polarity == PWMPolarity::Normal) { file << "normal"; }
    else if (polarity == PWMPolarity::Inverted) { file << "inverted"; }

    file.close();

    return PWMStatus::Success;
}

PWM::PWMStatus PWM::EnablePWM()
{
    return EnableDisablePWM(true);
}

PWM::PWMStatus PWM::DisablePWM()
{
    return EnableDisablePWM(false);
}

PWM::PWMStatus PWM::EnableDisablePWM(bool value)
{
    if (IsExported() != PWMStatus::Success) { return PWMStatus::NotExported; }

    std::ofstream file(m_pwmPath + "/pwm" + std::to_string(m_pwmChannel) + "/enable");

    if (!file) { return PWMStatus::Error; }

    file << std::to_string(value);
    file.close();

    return PWMStatus::Success;
}

PWM::PWMStatus PWM::IsExported()
{
    PWMStatus status = PWMStatus::NotExported;

    std::ifstream file(m_pwmPath + "/export");
    std::string line;

    if (std::getline(file, line))
    {
        if (line == std::to_string(m_pwmChannel)) { status = PWMStatus::Success; }
    }
    file.close();

    return status;
}

double PWM::Constrain(double value) const
{
    return std::max(std::min(value, m_maxDegrees), m_minDegrees);
}

size_t PWM::DegreesToPWM(double degrees)
{
    // Make sure value is within bounds, if bounds are set
    if (m_maxDegrees != 0 && m_minDegrees != 0) { degrees = Constrain(degrees); }

    // Map the degrees to pulse width linearly
    int pulseWidth = 900 + ((degrees + 60) * (2100 - 900)) / 120;

    return pulseWidth;
}
