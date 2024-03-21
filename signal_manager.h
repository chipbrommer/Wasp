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
#include "utilities/log_client.h"           // logger
// 
/////////////////////////////////////////////////////////////////////////////////

class SignalManager
{
public:
    /// @brief enum indicating available fins
    enum class FIN
    {
        ONE,
        TWO,
        THREE,
        FOUR
    };

    /// @brief Default constructor
    SignalManager(LogClient& logger);

    /// @brief Constructor
    /// @param fin1Path - Path to PWM for Fin 1
    /// @param fin1Channel - Channel number for Fin 1
    /// @param fin2Path - Path to PWM for Fin 1
    /// @param fin2Channel - Channel number for Fin 1 
    /// @param fin3Path - Path to PWM for Fin 1 
    /// @param fin3Channel - Channel number for Fin 1 
    /// @param fin4Path - Path to PWM for Fin 1 
    /// @param fin4Channel - Channel number for Fin 1 
    /// @param finMinDegrees - Value to set for Fin minimum degree location of movement 
    /// @param finMaxDegrees - Value to set for Fin maximum degree location of movement
    SignalManager(const std::string fin1Path, const int fin1Channel,
        const std::string fin2Path, const int fin2Channel,
        const std::string fin3Path, const int fin3Channel,
        const std::string fin4Path, const int fin4Channel,
        const double finMinDegrees, const double finMaxDegrees,
        LogClient& logger);

    /// @brief Default deconstructor
    ~SignalManager();

    /// @brief BLOCKING - Start an endless main working loop 
    void Start();

    /// @brief Stop the main working loop
    void Stop();

    /// @brief Ready a fin for use
    /// @param fin - The fin number to be readied
    /// @param finPath - The path to the PWM
    /// @param finChannel - The channel number for the PWM
    /// @param finMinDegrees - opt - Value to set for Fin minimum degree location of movement
    /// @param finMaxDegrees - opt - Value to set for Fin maximum degree location of movement
    /// @return true is successfully exported and enabled, otherwise false
    bool ReadyFin(const FIN fin, const std::string finPath, const int finChannel,
        const double finMinDegrees = 0, const double finMaxDegrees = 0);

    /// @brief Update a fins location to a specific degrees
    /// @param fin - The desired fin to be updated
    /// @param degrees - degrees value to sent to the fin
    bool UpdateFin_Degrees(const FIN fin, const double degrees);

protected:

private:
    std::atomic_bool mRun   = false;        /// Flag for running loop.           

    PWM mFin1;                              /// PWM connected to Fin 1
    PWM mFin2;                              /// PWM connected to Fin 2
    PWM mFin3;                              /// PWM connected to Fin 3
    PWM mFin4;                              /// PWM connected to Fin 4

    bool mFin1Ready         = false;        /// Indicator flag for Fin 1
    bool mFin2Ready         = false;        /// Indicator flag for Fin 2
    bool mFin3Ready         = false;        /// Indicator flag for Fin 3
    bool mFin4Ready         = false;        /// Indicator flag for Fin 4

    std::string m_name       = "SIG MGR";    /// Name for logging
    LogClient& m_logger;                     /// Logger
};