
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
#include "web_server.h"                     // header
// 
/////////////////////////////////////////////////////////////////////////////////

WebServer::WebServer(LogClient& logger, const int port, const std::string directory) :
    m_name("WEB SVR"), m_logger(logger),
    m_port(port), m_directory(directory), m_run(false), m_environment()
{
    // Inititalize libcivetweb
    mg_init_library(0);
}

WebServer::~WebServer() 
{ 
    Stop(); 
}

void WebServer::SetServerPort(int port)
{
    m_port = port;
}

void WebServer::SetServerDirectory(std::string directory)
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
    std::string port = std::to_string(m_port);
	const char* options[] = {
		"listening_ports", port.c_str(),
        "request_timeout_ms", "10000",
		"document_root", m_directory.c_str(),
		0
	};

    // Print any error messages to console
    struct mg_callbacks callbacks = { 0 };
    callbacks.log_message = [](const struct mg_connection* conn, const char* message) 
    {
        std::cerr << message << "\n";
        return 1;
    };

    // Log server stop
    m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Started on " + port);

    // Start the web server
    m_context = mg_start(&callbacks, 0, options);

    // Setup the request handler for any directory 
    mg_set_request_handler(m_context, "/$|/index$|/index.html$", [](struct mg_connection* c, void* cbdata) 
    {
        static_cast<WebServer*>(cbdata)->HandleConfigPage(c);
        return 0;
    }, this);

    // Setup the request handler for the reboot call
    mg_set_request_handler(m_context, "/reboot$", [](struct mg_connection* c, void* cbdata) 
    {
        static_cast<WebServer*>(cbdata)->HandleReboot(c);
        return 0;
    }, this);
}

void WebServer::Stop() 
{
    // Stop the web server
    if (m_context != nullptr) 
    {
        mg_stop(m_context);
        m_context = nullptr;
    }

    // Log server stop
    m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Stopped");
}

void WebServer::HandleConfigPage(mg_connection* c)
{
    // HTML content for the configuration page
    std::string htmlContent = "<h2>Configuration Page</h2>";
    htmlContent += "<p>Hello World</p>";

    // Send Content to the layout
    HandleLayoutPage(c, htmlContent);
}

void WebServer::HandleLayoutPage(mg_connection* c, std::string bodyContent)
{
    // HTML content for the layout page
    std::string htmlContent = "<html><body>";
    htmlContent += "<h1>Layout Page</h1>";
    htmlContent += bodyContent; // Embedding body content
    htmlContent += "</body></html>";

    // Send HTTP response with HTML content
    mg_printf(c, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %lu\r\n\r\n", htmlContent.size());
    mg_write(c, htmlContent.c_str(), htmlContent.size());
}

void WebServer::HandleReboot(mg_connection* c)
{
    m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Received reboot request.");

    // HTML content for the configuration page
    std::string htmlContent = "<h2>Reboot Command Received</h2>";
    htmlContent += "<p>Please wait for reconnection...</p>";

    // Send Content to the layout
    HandleLayoutPage(c, htmlContent);
}

std::string WebServer::Parse(const std::string name, const char* data)
{
    std::string value = "";
    try
    {
        std::string query_string(data);
        if (mg_get_var(data, query_string.size(), name.c_str(), m_buffer, BUFFER_SIZE) > 0)
        {
            value = std::string(m_buffer);

            // check if string is just whitespace
            if (value.find_first_not_of(' ') == std::string::npos)
            {
                value = "";
            }
        }
    }
    catch (...) {}

    return value;
}