
/////////////////////////////////////////////////////////////////////////////////
// @file            web_server.cpp
// @brief           Implementation for web server
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
// Includes:
//          name                            reason included
//          ------------------              ------------------------
#include <string>                           // strings
//
#include "web_server.h"                     // header
// 
/////////////////////////////////////////////////////////////////////////////////

void WebServer::SetPort(int port)
{
    m_port = port;
}

void WebServer::SetDirectory(std::string directory)
{
    m_directory = directory;
}

void WebServer::Configure(int port, std::string directory)
{
    m_port = port;
    m_directory = directory;
}

void WebServer::Start()
{
	// Configure civetweb options
	const char* options[] = {
		"listening_ports", std::to_string(m_port).c_str(),
		"document_root", m_directory.c_str(),
		0
	};

    m_running = true;

    while (m_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void WebServer::Stop() 
{
    if (m_server != nullptr) 
    {
        // Stop the server
        delete m_server;
        m_server = nullptr;

        // Reset running flag
        m_running = false;

        // Log server stop
        m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Stopped.");
    }
}

void WebServer::HandleConfigPage(mg_connection* c)
{

}

void WebServer::HandleLayoutPage(mg_connection* c, std::string bodyContent)
{

}

void WebServer::HandleReboot(mg_connection* c)
{

}

std::string WebServer::Parse(const std::string name, const char* data)
{
    return "";
}