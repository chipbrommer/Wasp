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

SignalManager::SignalManager(LogClient& logger) : mFin1(""), mFin2(""), mFin3(""), mFin4(""), m_logger(logger)
{}

SignalManager::SignalManager(const std::string fin1Path, const int fin1Channel,
    const std::string fin2Path, const int fin2Channel,
    const std::string fin3Path, const int fin3Channel,
    const std::string fin4Path, const int fin4Channel,
    const double finMinDegrees, const double finMaxDegrees, LogClient& logger
) : SignalManager(logger)
{
    ReadyFin(FIN::ONE,      fin1Path, fin1Channel, finMinDegrees, finMaxDegrees);
    ReadyFin(FIN::TWO,      fin2Path, fin2Channel, finMinDegrees, finMaxDegrees);
    ReadyFin(FIN::THREE,    fin3Path, fin3Channel, finMinDegrees, finMaxDegrees);
    ReadyFin(FIN::FOUR,     fin4Path, fin4Channel, finMinDegrees, finMaxDegrees);
}

/// @brief Deconstructor. UnExports PWMs. 
SignalManager::~SignalManager()
{
    // Unexport the PWMs
    if (mFin1.UnExportPWM() == PWM::PWMStatus::Error) m_logger.AddLog(m_name, LogClient::LogLevel::ERROR, "Fin1 failed to Unexport.");
    if (mFin2.UnExportPWM() == PWM::PWMStatus::Error) m_logger.AddLog(m_name, LogClient::LogLevel::ERROR, "Fin2 failed to Unexport.");
    if (mFin3.UnExportPWM() == PWM::PWMStatus::Error) m_logger.AddLog(m_name, LogClient::LogLevel::ERROR, "Fin3 failed to Unexport.");
    if (mFin4.UnExportPWM() == PWM::PWMStatus::Error) m_logger.AddLog(m_name, LogClient::LogLevel::ERROR, "Fin4 failed to Unexport.");
}

void SignalManager::Start()
{
    mRun = true;

    while (mRun)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void SignalManager::Stop()
{
    if (mRun)
    {
        // Create a notice and write it. 
        m_logger.AddLog(m_name, LogClient::LogLevel::INFO, "Closing.");

        // Signal the run to stop
        mRun = false;
    }
}

bool SignalManager::ReadyFin(const FIN fin, const std::string finPath, const int finChannel,
    const double finMinDegrees, const double finMaxDegrees)
{
    // Holder for status return
    PWM::PWMStatus status = PWM::PWMStatus::InvalidInput;

    switch (fin)
    {
    case FIN::ONE:
        mFin1.SetPath(finPath);
        mFin1.SetChannel(finChannel);
        mFin1.UpdateDegreeClamp(finMinDegrees, finMaxDegrees);

        if (mFin1.ExportPWM() == PWM::PWMStatus::Error)
        {
            m_logger.AddLog(m_name, LogClient::LogLevel::ERROR, "Fin1 failed to export.");
            return false;
        }

        status = mFin1.EnablePWM();
        if (status == PWM::PWMStatus::Error)            m_logger.AddLog(m_name, LogClient::LogLevel::ERROR, "Fin1 failed to enable.");
        else if (status == PWM::PWMStatus::NotExported) m_logger.AddLog(m_name, LogClient::LogLevel::ERROR, "Fin1 not exported.");
        else mFin1Ready = true;
        
        return mFin1Ready;

    case FIN::TWO:
        mFin2.SetPath(finPath);
        mFin2.SetChannel(finChannel);
        mFin2.UpdateDegreeClamp(finMinDegrees, finMaxDegrees);

        if (mFin2.ExportPWM() == PWM::PWMStatus::Error) 
        {
            m_logger.AddLog(m_name, LogClient::LogLevel::ERROR, "Fin2 failed to export.");
            return false;
        }

        status = mFin2.EnablePWM();
        if (status == PWM::PWMStatus::Error)            m_logger.AddLog(m_name, LogClient::LogLevel::ERROR, "Fin2 failed to enable.");
        else if (status == PWM::PWMStatus::NotExported) m_logger.AddLog(m_name, LogClient::LogLevel::ERROR, "Fin2 not exported.");
        else mFin2Ready = true;

        return mFin2Ready;

    case FIN::THREE:
        mFin3.SetPath(finPath);
        mFin3.SetChannel(finChannel);
        mFin3.UpdateDegreeClamp(finMinDegrees, finMaxDegrees);

        if (mFin3.ExportPWM() == PWM::PWMStatus::Error) 
        {
            m_logger.AddLog(m_name, LogClient::LogLevel::ERROR, "Fin3 failed to export.");
            return false;
        }

        status = mFin3.EnablePWM();
        if (status == PWM::PWMStatus::Error)            m_logger.AddLog(m_name, LogClient::LogLevel::ERROR, "Fin3 failed to enable.");
        else if (status == PWM::PWMStatus::NotExported) m_logger.AddLog(m_name, LogClient::LogLevel::ERROR, "Fin3 not exported.");
        else mFin3Ready = true;

        return mFin3Ready;

    case FIN::FOUR:
        mFin4.SetPath(finPath);
        mFin4.SetChannel(finChannel);
        mFin4.UpdateDegreeClamp(finMinDegrees, finMaxDegrees);

        if (mFin4.ExportPWM() == PWM::PWMStatus::Error)
        {
            m_logger.AddLog(m_name, LogClient::LogLevel::ERROR, "Fin4 failed to export.");
            return false;
        }

        status = mFin4.EnablePWM();
        if (status == PWM::PWMStatus::Error)            m_logger.AddLog(m_name, LogClient::LogLevel::ERROR, "Fin4 failed to enable.");
        else if (status == PWM::PWMStatus::NotExported) m_logger.AddLog(m_name, LogClient::LogLevel::ERROR, "Fin4 not exported.");
        else mFin4Ready = true;

        return mFin4Ready;

    default:
        return false;
    }
}

bool SignalManager::UpdateFin_Degrees(const FIN fin, const double degrees)
{
    switch (fin)
    {
    case FIN::ONE:
        break;
    case FIN::TWO:
        break;
    case FIN::THREE:
        break;
    case FIN::FOUR:
        break;
    }

    return true;
}