/////////////////////////////////////////////////////////////////////////////////
// @file            signal_manager.cpp
// @brief           Implementation for signal manager
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
//
#include "signal_manager.h"                 // header
// 
/////////////////////////////////////////////////////////////////////////////////

/// @brief Default constructor
SignalManager::SignalManager() : mFin1(""), mFin2(""), mFin3(""), mFin4("")
{}

/// @brief Constructor
/// @param fin1Path - Path to PWM for Fin 1
/// @param fin1Channel - Channel number for Fin 1
/// @param fin2Path - Path to PWM for Fin 1
/// @param fin2Channel - Channel number for Fin 1 
/// @param fin3Path - Path to PWM for Fin 1 
/// @param fin3Channel - Channel number for Fin 1 
/// @param fin4Path - Path to PWM for Fin 1 
/// @param fin4Channel - Channel number for Fin 1 
/// @param finMinDegrees - opt - Value to set for Fin minimum degree location of movement 
/// @param finMaxDegrees - opt - Value to set for Fin maximum degree location of movement
SignalManager::SignalManager(const std::string fin1Path, const int fin1Channel,
    const std::string fin2Path, const int fin2Channel,
    const std::string fin3Path, const int fin3Channel,
    const std::string fin4Path, const int fin4Channel,
    const double finMinDegrees, const double finMaxDegrees
) : mFin1(fin1Path, fin1Channel, finMinDegrees, finMaxDegrees),
    mFin2(fin2Path, fin2Channel, finMinDegrees, finMaxDegrees),
    mFin3(fin3Path, fin3Channel, finMinDegrees, finMaxDegrees),
    mFin4(fin4Path, fin4Channel, finMinDegrees, finMaxDegrees)
{
    // Make sure the PWMs are exported. 
    if (mFin1.ExportPWM() == PWM::PWMStatus::Error) std::cerr << "[SIG MGR] - Error - Fin1 failed to export.";
    if (mFin2.ExportPWM() == PWM::PWMStatus::Error) std::cerr << "[SIG MGR] - Error - Fin2 failed to export.";
    if (mFin3.ExportPWM() == PWM::PWMStatus::Error) std::cerr << "[SIG MGR] - Error - Fin3 failed to export.";
    if (mFin4.ExportPWM() == PWM::PWMStatus::Error) std::cerr << "[SIG MGR] - Error - Fin4 failed to export.";

    // Enable the PWMs
    PWM::PWMStatus status = PWM::PWMStatus::InvalidInput;

    status = mFin1.EnablePWM();
    if (status == PWM::PWMStatus::Error)            std::cerr << "[SIG MGR] - Error - Fin1 failed to enable.";
    else if (status == PWM::PWMStatus::NotExported) std::cerr << "[SIG MGR] - Error - Fin1 not exported.";

    status = mFin2.EnablePWM();
    if (status == PWM::PWMStatus::Error)            std::cerr << "[SIG MGR] - Error - Fin2 failed to enable.";
    else if (status == PWM::PWMStatus::NotExported) std::cerr << "[SIG MGR] - Error - Fin2 not exported.";

    status = mFin3.EnablePWM();
    if (status == PWM::PWMStatus::Error)            std::cerr << "[SIG MGR] - Error - Fin3 failed to enable.";
    else if (status == PWM::PWMStatus::NotExported) std::cerr << "[SIG MGR] - Error - Fin3 not exported.";

    status = mFin4.EnablePWM();
    if (status == PWM::PWMStatus::Error)            std::cerr << "[SIG MGR] - Error - Fin4 failed to enable.";
    else if (status == PWM::PWMStatus::NotExported) std::cerr << "[SIG MGR] - Error - Fin4 not exported.";
}

/// @brief Deconstructor. UnExports PWMs. 
SignalManager::~SignalManager()
{
    // Unexport the PWMs
    if (mFin1.UnExportPWM() == PWM::PWMStatus::Error) std::cerr << "[SIG MGR] - Error - Fin1 failed to unexport.";
    if (mFin2.UnExportPWM() == PWM::PWMStatus::Error) std::cerr << "[SIG MGR] - Error - Fin2 failed to unexport.";
    if (mFin3.UnExportPWM() == PWM::PWMStatus::Error) std::cerr << "[SIG MGR] - Error - Fin3 failed to unexport.";
    if (mFin4.UnExportPWM() == PWM::PWMStatus::Error) std::cerr << "[SIG MGR] - Error - Fin4 failed to unexport.";
}

void SignalManager::Start()
{

}

bool SignalManager::UpdateFin_Degrees(const FIN fin, const double degrees)
{
    return true;
}