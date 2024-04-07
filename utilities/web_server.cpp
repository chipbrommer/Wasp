
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

WebServer::WebServer(LogClient& logger, const int port, const std::string directory, 
    const int maxThreads) :
    m_name("WEB SVR"), m_logger(logger), m_port(port), m_directory(directory),
    m_run(false), m_environment(), m_maxThreads(maxThreads)
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
        "num_threads", "5",
        "request_timeout_ms", "10000",
        "document_root", m_directory.c_str(),
        0
    };

    static const char subprotocol_bin[] = "Company.ProtoName.bin";
    static const char subprotocol_json[] = "Company.ProtoName.json";
    static const char* subprotocols[] = { subprotocol_bin, subprotocol_json, NULL };
    static struct mg_websocket_subprotocols wsprot = { 2, subprotocols };

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
        return 1;
    }, this);

    // Setup the request handler for the reboot call
    mg_set_request_handler(m_context, "/data$", [](struct mg_connection* c, void* cbdata)
    {
        static_cast<WebServer*>(cbdata)->HandlePage(c, Page::Data);
        return 1;
    }, this);

    // Setup the request handler for any directory 
    mg_set_request_handler(m_context, "/$|/index$|/index.html$", [](struct mg_connection* c, void* cbdata)
    {
        static_cast<WebServer*>(cbdata)->HandlePage(c, Page::Index);
        return 1;
    }, this);

    // Setup the request handler for the login page
    mg_set_request_handler(m_context, "/login$", [](struct mg_connection* c, void* cbdata)
    {
        // Extract the POST data from the request
        char post_data[WEB_BUFFER_SIZE];
        int post_data_len = mg_read(c, post_data, sizeof(post_data));

        // Ensure that the data is null-terminated
        post_data[post_data_len] = '\0';

        // Parse the POST data to get the password
        char password[WEB_BUFFER_SIZE];
        if (mg_get_var(post_data, post_data_len, "password", password, sizeof(password)) > 0)
        {
            // Check the password
            if (strcmp(password, "password") == 0)
            {
                // Password is correct, redirect to dev page
                static_cast<WebServer*>(cbdata)->HandlePage(c, Page::Dev);
                return 1;
            }
            else
            {
                // Password is incorrect, display an error message on the login page
                std::string errorMessage = "<p style=\"color:red;\">Incorrect password. Please try again.</p>";
                std::string loginPageWithError = loginPage;
                size_t pos = loginPageWithError.find("</form>");
                if (pos != std::string::npos) {
                    loginPageWithError.insert(pos, errorMessage);
                }

                static_cast<WebServer*>(cbdata)->HandleLayoutPage(c, loginPageWithError);
                return 1;
            }
        }

        // No password provided, this will just show the login page
        static_cast<WebServer*>(cbdata)->HandlePage(c, Page::Login);
        return 1; // Indicate that the request has been handled
    }, this);

    // Setup the request handler for the reboot call
    mg_set_request_handler(m_context, "/reboot$", [](struct mg_connection* c, void* cbdata)
    {
        static_cast<WebServer*>(cbdata)->HandlePage(c, Page::Reboot);
        return 1;
    }, this);

    // Setup the request handler for the reboot call
    mg_set_request_handler(m_context, "/update$", [](struct mg_connection* c, void* cbdata)
        {
            static_cast<WebServer*>(cbdata)->HandlePage(c, Page::Update);
            return 1;
        }, this);

    // Setup a default handler for any unmatched requests
    mg_set_request_handler(m_context, "/*", [](struct mg_connection* c, void* cbdata)
    {
        static_cast<WebServer*>(cbdata)->HandlePage(c, Page::Error);
        return 1;
    }, this);

    // Start the websocket
    //mg_set_websocket_handler_with_subprotocols(m_context,
    //    "/websocket",
    //    &wsprot,
    //    ws_connect_handler,
    //    ws_ready_handler,
    //    ws_data_handler,
    //    ws_close_handler,
    //    user_data);
}

void WebServer::SendJsonOverWebSocket(const nlohmann::json& json) 
{
    std::string message = json.dump(); // Convert JSON to string
    SendMessageOverWebSocket(message);
}

void WebServer::SendMessageOverWebSocket(const std::string& msg)
{
    std::lock_guard<std::mutex> lock(m_connectionLock);

    // Iterate through connections and send message to each WebSocket client
    for (auto& pair : m_connections)
    {
        struct mg_connection* conn = pair.first;

        /*if (mg_websocket_write(conn, MG_WEBSOCKET_OPCODE_TEXT, msg.c_str(), msg.length()) <= 0)
        {
            std::cerr << "Failed to send message over WebSocket\n";
        }*/
    }
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
    case Page::Data:
        HandleLayoutPage(c, dataPage);
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
    case Page::Update:
        m_logger.AddLog(m_name, LogClient::LogLevel::Info, "Received update request.");
        HandleLayoutPage(c, updatePage);
        break;
    case Page::Error:
    default:
        HandleLayoutPage(c, errorPage);
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

int WebServer::WebSocketConnectHandler(const struct mg_connection* conn, void* user_data) 
{
    WebServer* server = static_cast<WebServer*>(user_data);
    std::lock_guard<std::mutex> lock(server->m_connectionLock);
    //server->m_connections[conn] = true;
    return 1;
}

void WebServer::WebSocketReadyHandler(struct mg_connection* conn, void* user_data) 
{
    // WebSocket connection is ready for sending/receiving data
}

int WebServer::WebsocketDataHandler(struct mg_connection* conn, int bits, char* data, size_t data_len, void* user_data) 
{
    // Only sending for now
    return 1;
}

void WebServer::WebSocketCloseHandler(const struct mg_connection* conn, void* user_data) 
{
    WebServer* server = static_cast<WebServer*>(user_data);
    std::lock_guard<std::mutex> lock(server->m_connectionLock);
    server->m_connections.erase(const_cast<struct mg_connection*>(conn));
}