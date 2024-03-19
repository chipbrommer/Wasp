#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            signal_manager.h
// @brief           A manager for controlling signals for Wasp
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
//
#include "utilities/pwm_interface.h"        // pwms
// 
/////////////////////////////////////////////////////////////////////////////////

class SignalManager
{
public:
    enum class FIN
    {
        ONE,
        TWO,
        THREE,
        FOUR
    };

    SignalManager();

    SignalManager(const std::string fin1Path, const int fin1Channel, 
        const std::string fin2Path, const int fin2Channel,
        const std::string fin3Path, const int fin3Channel,
        const std::string fin4Path, const int fin4Channel,
        const double finMinDegrees = 0, const double finMaxDegrees = 0
        );

    /// @brief Default deconstructor
    ~SignalManager();

    /// @brief BLOCKING - Start an endless main working loop 
    void Start();

    /// @brief Update a fins location to a specific degrees
    /// @param fin - The desired fin to be updated
    /// @param degrees - degrees value to sent to the fin
    bool UpdateFin_Degrees(const FIN fin, const double degrees);

protected:

private:
    PWM mFin1;                              /// PWM connected to Fin 1
    PWM mFin2;                              /// PWM connected to Fin 2
    PWM mFin3;                              /// PWM connected to Fin 3
    PWM mFin4;                              /// PWM connected to Fin 4

};