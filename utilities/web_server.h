#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            web_server.h
// @brief           Class to implement a web server
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// 
// Includes:
//          name                    reason included
//          ------------------      ------------------------
#include <string>                   // strings
//
#include "../external/civetweb/CivetServer.h"   // Civet header
#include "log_client.h"             // Log Client
//
/////////////////////////////////////////////////////////////////////////////////

class WebServer
{
public:

    WebServer(LogClient& logger, const int port = 8080, const std::string directory = ".") :
        m_name("WEB SVR"), m_server(nullptr), m_logger(logger), 
        m_port(port), m_directory(directory), m_running(false) {}
    
    ~WebServer() { Stop(); }

    void SetPort(int port);

    void SetDirectory(std::string path);

    void Start();

    void Stop();

protected:

private:

    std::string     m_name;
    CivetServer*    m_server;
    LogClient&      m_logger;
    int             m_port;
    std::string     m_directory;
    bool            m_running;
    const mg_context* m_context;
};