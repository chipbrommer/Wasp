
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

    // Setup the request handler for the reboot call
    mg_set_request_handler(m_context, "/config$", [](struct mg_connection* c, void* cbdata)
        {
            static_cast<WebServer*>(cbdata)->HandlePage(c, Page::Config);
            return 0;
        }, this);

    // Setup the request handler for any directory 
    mg_set_request_handler(m_context, "/$|/index$|/index.html$", [](struct mg_connection* c, void* cbdata)
        {
            static_cast<WebServer*>(cbdata)->HandlePage(c, Page::Index);
            return 0;
        }, this);

    // Setup the request handler for the reboot call
    mg_set_request_handler(m_context, "/login$", [](struct mg_connection* c, void* cbdata)
        {
            static_cast<WebServer*>(cbdata)->HandlePage(c, Page::Login);
            return 0;
        }, this);

    // Setup the request handler for the reboot call
    mg_set_request_handler(m_context, "/reboot$", [](struct mg_connection* c, void* cbdata)
        {
            static_cast<WebServer*>(cbdata)->HandlePage(c, Page::Reboot);
            return 0;
        }, this);

    // Setup the request handler for the login form submission
    //mg_set_request_handler(m_context, "/login$", [](struct mg_connection* c, void* cbdata)
    //{
    //    auto* server = static_cast<WebServer*>(cbdata);

    //    // Retrieve the POST data from the request
    //    char post_data[1000];
    //    int post_data_len = mg_read(c, post_data, sizeof(post_data));

    //    // Extract the password parameter from the POST data
    //    char password[100];
    //    if (mg_get_http_var(post_data, post_data_len, "password", password, sizeof(password)) > 0) {
    //        // Check if the password is correct
    //        if (strcmp(password, "your_password_here") == 0) {
    //            // Password is correct, display the form page
    //            server->HandlePage(formPage);
    //        }
    //        else {
    //            // Password is incorrect, display the login page with an error message
    //            std::string errorMessage = "<p style=\"color: red;\">Incorrect password. Please try again.</p>";
    //            std::string formattedLoginPage =
    //                loginPage.replace(loginPage.find("%s"), 2, errorMessage);
    //            server->HandlePage(formattedLoginPage);
    //        }
    //    }
    //    else {
    //        // Password parameter not found, display the login page without any error message
    //        server->HandlePage(loginPage);
    //    }

    //    return 0;
    //}, this);

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

void WebServer::HandlePage(mg_connection* c, Page page)
{
    switch (page)
    {
    case Page::Config:
        HandleLayoutPage(c, configPage);
        break;
    case Page::Dev:
        HandleLayoutPage(c, devPage);
        break;
    case Page::Index:
        HandleLayoutPage(c, indexPage);
        break;
    case Page::Login:
        HandleLayoutPage(c, loginPage);
        break;
    case Page::Reboot:
        m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Received reboot request.");
        HandleLayoutPage(c, rebootPage);
        break;
    case Page::Error:
    default:
        break;
    };
}

void WebServer::HandleConfigPage(mg_connection* c)
{
    //   const struct mg_request_info* ri = mg_get_request_info(c);

    //   std::string method = strtok(const_cast<char*>(ri->request_method), " ");
    //   std::string typeValue = Parse("type", ri->query_string);

       //std::string configContent = "";
       //bool isSaveSuccessful = false;
       //bool didSomeNotSave = false;
       //bool isSaveFailed = false;
       //if (typeValue != "")
       //{
       //	std::string handler = buffer;
       //	if (handler == "system")			fields = systemFields;

       //	nlohmann::json json = { };

       //	if (method == "GET")
       //	{
       //		for (std::shared_ptr<Field> field : fields)
       //		{
       //			json[field->name] = { { "isgood", true } };
       //			field->setjson(json[field->name]["value"]);
       //		}
       //	}
       //	else
       //	{
       //		bool didAnySave = false;

       //		char post_data[1024];
       //		int post_data_len = mg_read(c, post_data, 1024);

       //		for (std::shared_ptr<Field> field : fields)
       //		{
       //			std::string value = Parse(field->name, post_data);
       //			json[field->name] = { { "isgood", field->parseAndValidate(value) } };
       //			if (json[field->name]["isgood"])
       //			{
       //				field->setjson(json[field->name]["value"]);
       //				didAnySave = true;
       //			}
       //			else
       //			{
       //				json[field->name]["value"] = value;
       //				didSomeNotSave = true;
       //			}
       //		}

       //		if (saveProgramConfiguration(defaultConfigPath))
       //		{
       //			isSaveSuccessful = true;

       //			// update sal code decimal.
       //			std::istringstream iss(programConfig.salCodeHex);
       //			iss >> std::hex >> programConfig.salCodeDec;
       //		}

       //		if (!didAnySave)
       //			isSaveFailed = true;
       //	}

       //	if (handler == "system")			configContent = m_environment.render(configPage_systemConfigPage, json);
    //       
       //}

       //nlohmann::json json = {
       //	{ "configContent", configContent },
       //	{ "isSaveSuccessful", isSaveSuccessful },
       //	{ "didSomeNotSave", didSomeNotSave },
       //	{ "isSaveFailed", isSaveFailed }
       //};

       //std::string bodyContent = m_environment.render(configPage, json);
       //HandleLayoutPage(c, bodyContent);
}

void WebServer::HandleLayoutPage(mg_connection* c, std::string bodyContent)
{
    nlohmann::json json = {
        { "bodyContent", bodyContent },
        { "version", std::to_string(PROJECT_VERSION_MAJOR) + "." +
            std::to_string(PROJECT_VERSION_MINOR) + "." +
            std::to_string(PROJECT_VERSION_PATCH) + " (" +
            PROJECT_GIT_COMMIT_HASH + ")"
        }
    };
    const std::string wholePage = m_environment.render(layoutPage, json);

    mg_send_http_ok(c, "text/html", wholePage.size());
    mg_write(c, wholePage.c_str(), wholePage.size());
}

std::string WebServer::Parse(const std::string name, const char* data)
{
    std::string value = "";
    try
    {
        std::string query_string(data);
        if (mg_get_var(data, query_string.size(), name.c_str(), m_buffer, 50) > 0)
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