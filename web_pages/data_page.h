#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            data_page.h
// @brief           data html content in a std::string
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
//  Include files:
//          name                        reason included
//          --------------------        ---------------------------------------
#include <string>                       // strings
//
/////////////////////////////////////////////////////////////////////////////////

const std::string dataPage = R"(
    <div id="messages"></div> <!-- Display received WebSocket messages here -->
    <form id="dataForm">
        <label for="time">Time (HH:MM:SS):</label>
        <input type="text" id="hour" name="hour" readonly style="width: 30px;">:
        <input type="text" id="min" name="min" readonly style="width: 30px;">:
        <input type="text" id="sec" name="sec" readonly style="width: 30px;"><br>
        <label for="latitude">Latitude:</label>
        <input type="number" id="latitude" name="latitude" readonly><br>
        <label for="longitude">Longitude:</label>
        <input type="number" id="longitude" name="longitude" readonly><br>
        <label for="altitude">Altitude:</label>
        <input type="number" id="altitude" name="altitude" readonly><br>
    </form>
    <script>
        // Create a WebSocket connection
        var ws = new WebSocket("ws://localhost:" + window.location.port + "/websocket");

        // WebSocket event handler for message reception
        ws.onmessage = function(event) {
            // Parse received JSON data
            var message = JSON.parse(event.data);
            
            // Check if message follows the "data" schema
            if (message.hasOwnProperty("data"))
            {
                if(message.data.hasOwnProperty("latitude")) { document.getElementById("latitude").value = message.data.latitude; }
                if(message.data.hasOwnProperty("longitude")) { document.getElementById("longitude").value = message.data.longitude; }
                if(message.data.hasOwnProperty("altitude")) { document.getElementById("altitude").value = message.data.altitude; }
                if(message.data.hasOwnProperty("hour")) { document.getElementById("hour").value = message.data.hour; }
                if(message.data.hasOwnProperty("min")) { document.getElementById("min").value = message.data.min; }
                if(message.data.hasOwnProperty("sec")) { document.getElementById("sec").value = message.data.sec; }
            }

            // Check if message has any new messages
            if (message.hasOwnProperty("message"))
            {
                // Check if message has any new messages
                if (message.message.hasOwnProperty("text")) 
                {
                    // Display received JSON message with timestamp
                    var receivedTime = new Date().toLocaleTimeString();
                    var messagesDiv = document.getElementById("messages");
                    messagesDiv.innerHTML = "<p>Received at " + receivedTime + ": " + JSON.stringify(message.message.text) + "</p>";
                }

                if (message.message.hasOwnProperty("timeout"))
                {
                    // Start a timer to clear the content after timeout
                    setTimeout(function() {
                        messagesDiv.innerHTML = ""; // Clear the content
                    }, message.message.timeout * 1000);
                }
            }
        };

        // WebSocket event handler for connection open
        ws.onopen = function(event) {
            console.log("WebSocket connection opened");
        };

        // WebSocket event handler for connection close
        ws.onclose = function(event) {
            console.log("WebSocket connection closed");
        };

        // WebSocket event handler for errors
        ws.onerror = function(event) {
            console.error("WebSocket error:", event);
        };
    </script>
)";
