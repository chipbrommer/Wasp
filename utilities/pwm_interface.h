#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            pwm_interface.cpp
// @brief           Class to interface with system pwm's
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// 
// Includes:
//          name                    reason included
//          ------------------      ------------------------
#include <string>                   // strings
#include <iostream>                 // console io
#include <fstream>                  // file io
#include <algorithm>                // min and max
//
/////////////////////////////////////////////////////////////////////////////////

class PWM
{
public:
    /// @brief enum for status returns
    enum class PWMStatus
    {
        Success,
        Error,
        NotExported,
        InvalidInput,
    };

    /// @brief enum for PWM polarity types
    enum class PWMPolarity
    {
        Normal,
        Inverted,
        Unknown
    };

    /// @brief Constructor
    /// @param pwmPath - Path to the pwm Typically "/sys/class/pwm/pwmchipX"
    /// @param pwmChannel - [opt] - Specific channel on the PWM, typically not needed - defaults to 0
    /// @param pwmMinDegrees - [opt] - Adds ability to clamp the degrees to pwm via this minimum degree spec. 
    /// @param pwmMaxDegrees - [opt] - Adds ability to clamp the degrees to pwm via this max degree spec.
    PWM(const std::string& pwmPath, const int pwmChannel = 0, const double pwmMinDegrees = 0, const double pwmMaxDegrees = 0)
        : m_pwmPath(pwmPath), m_pwmChannel(pwmChannel), m_minDegrees(pwmMinDegrees), m_maxDegrees(pwmMaxDegrees) {}

    /// @brief Sets the path for the PWM
    /// @param pwmPath - Path to the pwm Typically "/sys/class/pwm/pwmchipX"
    /// @return true if successfully set, otherwise false
    bool SetPath(const std::string& pwmPath);

    /// @brief Sets the pwm channel
    /// @param pwmChannel - Specific channel on the PWM, typically not needed
    /// @return true if successfully set, otherwise false
    bool SetChannel(const int pwmChannel);

    /// @brief Updates the degree clamp for the PWM signal if sending degrees value
    /// @param pwmMinDegrees - Adds ability to clamp the degrees to pwm via this minimum degree spec.
    /// @param pwmMaxDegrees - Adds ability to clamp the degrees to pwm via this max degree spec.
    /// @return true if successfully set, otherwise false
    bool UpdateDegreeClamp(const double pwmMinDegrees, const double pwmMaxDegrees);

    /// @brief - export a PWM for usage
    /// @return - PWMStatus::Success if successful export, else PWMStatus::Error.
    PWMStatus ExportPWM();

    /// @brief - unexport a PWM
    /// @return - PWMStatus::Success if successful unexport or already unexported, else PWMStatus::Error.
    PWMStatus UnExportPWM();

    /// @brief - Get the period of a PWM
    /// @param period_ns - output variable for return
    /// @return - PWMStatus::Success if successful period read, else PWMStatus::Error.
    PWMStatus GetPeriod(size_t& period_ns);

    /// @brief - Get the PWM period 
    /// @return - period of PWM on sucessful read, else -1
    size_t GetPeriod();

    /// @brief - Set the PWM period
    /// @param period_ns - value to be set as period (Nanoseconds)
    /// @return - PWMStatus::Success if successful set of period, else PWMStatus::Error.
    PWMStatus SetPeriod(size_t period_ns);

    /// @brief - Get the PWM duty cycle
    /// @param dutyCycle_ns - output variable for return
    /// @return - PWMStatus::Success if successful duty cycle read, else PWMStatus::Error.
    PWMStatus GetDutyCycle(size_t& dutyCycle_ns);

    /// @brief - Get the PWM Duty Cycle 
    /// @return - Duty cycle of PWM on sucessful read, else -1
    size_t GetDutyCycle();

    /// @brief - Set the PWM Duty Cycle 
    /// @param dutyCycle_ns - value to be set as duty cycle (Nanoseconds)
    /// @return - PWMStatus::Success if successful set of duty cycle, else PWMStatus::Error.
    PWMStatus SetDutyCycle(int dutyCycle_ns);

    /// @brief - Get the PWM polarity
    /// @param polarity - output variable for return
    /// @return - PWMStatus::Success if successful polarity read, else PWMStatus::Error.
    PWMStatus GetPolarity(PWMPolarity& polarity);

    /// @brief - Get the PWM polarity
    /// @return - Polarity type based on read, else PWMPolarity::Unknwon
    PWMPolarity GetPolarity();

    /// @brief - Set the PWM polarity
    /// @param polarity - Desired polatiry type for the PWM
    /// @return - PWMStatus::InvalidInput if PWMPolarity::Unknown received, PWMStatus::Success on good set, else PWMStatus::Error
    PWMStatus SetPolarity(const PWMPolarity polarity);

    /// @brief - Enable the PWM
    /// @return - PWMStatus::Sucess if good set, PWMStatus::NotExported if not exported, else PWMStatus::Error
    PWMStatus EnablePWM();

    /// @brief - Disable the PWM
    /// @return - PWMStatus::Sucess if good set, PWMStatus::NotExported if not exported, else PWMStatus::Error
    PWMStatus DisablePWM();

private:
    std::string m_pwmPath;      // path of the pwm
    int         m_pwmChannel;   // channel of the pwm
    double      m_maxDegrees;   // maximum degree range for the pwm
    double      m_minDegrees;   // minimum degree range for the pwm

    /// @brief function to enable or disable a PWM
    /// @param value - bool to enable (true) or disable (false) PWM
    /// @return - PWMStatus::Sucess if good set, PWMStatus::NotExported if not exported, else PWMStatus::Error
    PWMStatus EnableDisablePWM(bool value);

    /// @brief Validate that a PWM is exported
    /// @return PWMStatus::Success if exported, else PWMStatus::NotExported
    PWMStatus IsExported();

    /// @brief Constrains a value within the MIN and MAX values
    /// @param value - value to be checked / constrained
    /// @return - constrained value
    double Constrain(double value) const;

    /// @brief Convert degrees into A valid PWM signal, Will constrain to min/max range if thy're not zero. 
    /// @param degrees - input for desired degree of input
    /// @return - converted PWM value
    size_t DegreesToPWM(double degrees);

};