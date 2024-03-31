#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            wasp_m.h
// @brief           A full operational flight software implementation
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
#include <cstdint>                          // standard ints
//
#include "files/settings.h"                 // program settings
#include "files/build.h"                    // build information
#include "files/configuration.h"            // configuration information
#include "utilities/json_file_utility.hpp"  // settings utility
#include "utilities/log_client.h"           // log client
#include "managers/signal_manager.h"        // signal manager
#include "imu/imu_manager.h"                // imu manager
#include "gps/gps_manager.h"                // gps manager
#include "utilities/cot_utility.h"          // cot messaging
#include "utilities/web_server.h"           // web server
// 
/////////////////////////////////////////////////////////////////////////////////

class Wasp
{
public:
    /// @brief Default constructure
    /// @param settingsLocation - Location of the settings file
    /// @param buildLocation - Location of the build file
    /// @param configLocation - Location of the configuration file
    Wasp(const std::string& settingsLocation, const std::string& buildLocation, const std::string& configLocation);

    /// @brief Default deconstructor
    ~Wasp();

    /// @brief Main entry point to start Wasp
    void Execute();

    /// @brief Graceful closing procedure
    void Close();

protected:

private:
    // Input Files
    JsonFileUtility<Settings>       m_settings;             /// Settings file 
    JsonFileUtility<Build>          m_build;                /// Build log file
    JsonFileUtility<Configuration>  m_config;               /// Configuration file

    // Clients / Managers
    std::string                     m_name;                 /// Name for when using logger.
    LogClient                       m_logger;               /// Client for logging to console and file
    SignalManager                   m_signalManger;         /// Manager for PWMs and GPIOs
    GpsManager                      m_gpsManager;           /// Manager for GPS units
    ImuManager                      m_imuManager;           /// Manager for IMU units
    WebServer                       m_webServer;            /// Web server interface

    // Utilities
    COT_Utility                     m_cot;                  /// Utility to generate and handle CoT stuff. 

    // Threads
    std::thread                     m_signalThread;         /// Thread for Signal manager
    std::thread                     m_loggingThread;        /// Thread for logging
    std::thread                     m_webThread;            /// Thread for web interface
    std::thread                     m_ProcessingThread;     /// Main thread for program processing

    /// Flags
    std::atomic_bool                m_run;                  /// Flag indicating we are good to run. 
    bool                            m_initialized;          /// Flag indicating initialization successful
};