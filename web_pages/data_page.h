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
    <form id="devForm">
        <label for="x">X:</label>
        <input type="text" id="x" name="x" readonly><br><br>
        <label for="y">Y:</label>
        <input type="text" id="y" name="y" readonly><br><br>
        <label for="z">Z:</label>
        <input type="text" id="z" name="z" readonly><br><br>
    </form>
    <script>
        var devForm = document.getElementById("devForm");

        // Create a WebSocket connection
        var ws = new WebSocket("ws://" + window.location.hostname + ":" + window.location.port + "/websocket");

        // WebSocket event handler for message reception
        ws.onmessage = function(event) {
            // Parse received JSON data
            var message = JSON.parse(event.data);
            
            // Check if message follows the "dev" schema
            if (message.hasOwnProperty("dev") && message.dev.hasOwnProperty("x") &&
                message.dev.hasOwnProperty("y") && message.dev.hasOwnProperty("z")) {
                // Populate form fields with x, y, and z values
                document.getElementById("x").value = message.dev.x;
                document.getElementById("y").value = message.dev.y;
                document.getElementById("z").value = message.dev.z;
            }

            // Display received JSON message
            var messagesDiv = document.getElementById("messages");
            messagesDiv.innerHTML += "<p>Received: " + JSON.stringify(message) + "</p>";
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

        // Form submit event handler
        devForm.addEventListener("submit", function(event) {
            event.preventDefault(); // Prevent form submission

            // Extract values from form fields
            var xValue = document.getElementById("x").value;
            var yValue = document.getElementById("y").value;
            var zValue = document.getElementById("z").value;

            // Create a JSON object with the form data
            var formData = {
                "x": xValue,
                "y": yValue,
                "z": zValue
            };

            // Send the JSON data via WebSocket
            ws.send(JSON.stringify(formData));

            // Clear form fields
            devForm.reset();
        });
    </script>
)";
