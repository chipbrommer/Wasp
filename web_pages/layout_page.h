#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            layout_page.h
// @brief           layout html content in a std::string
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

const std::string layoutPage = R"(
	<html xmlns="http://www.w3.org/1999/xhtml" >
		<head>
			<title>Wasp-M</title>
			<style type="text/css">
                body {
                    margin: 0;
                    padding: 0;
                    min-width: 600px;
                    font-family: Verdana, Helvetica, Arial, sans-serif;
                    background-color: #1d2335;
                    color: #FFFFFF;
                }

                a:hover {
                    text-decoration: underline;
                    color: gray;
                }

                a {
                    text-decoration: none;
                    color: white;
                }

		        hr {
		            display: block;
		            margin-top: 0.5em;
		            margin-bottom: 0.5em;
		            margin-left: auto;
		            margin-right: auto;
		            border-style: inset;
		            border-width: 1px;
		        }

                #content {
                    padding: 15px;
                    min-height: 400px;
                    margin: auto;
                    display: flex;
                }

                #left-column {
                    flex: 1;
                    background-color: #131725;
                    border-radius: 10px;
                    padding: 10px;
                    margin-right: 10px;
                    display: flex;
                    justify-content: center;
                    align-items: top;
                }

                #left-column {
                    width: 14%;
                    background-color: #131725;
                    border-radius: 10px;
                    padding: 10px;
                    margin-right: 10px;
                }

                #right-column {
                    width: 86%;
                    background-color: #131725;
                    border-radius: 10px;
                    padding: 10px;
                }

                #header {
                    display: flex;
                    align-items: center;
                    justify-content: space-between;
                    font-size: 18px;
                    overflow: hidden;
                    border-bottom: 1px solid #FFFFFF;
                    padding: 10px 0;
                }

                #header table {
                    width: 100%;
                    min-width: 800px;
                    margin: 6px;
                }

                #header td:first-child {
                    padding-left: 50px;
                    text-align: left;
                }

                #header td:last-child {
                    text-align: right;
                    padding-right: 50px;
                }

                #header .program-name {
                    font-size: 24px;
                    font-weight: bold;
                    text-align: center;
                }

                #header .version {
                    font-size: 18px;
                    text-align: left;
                    padding-left: 50px;
                }

                #header .reboot {
                    font-size: 18px;
                    text-align: right;
                    padding-right: 50px;
                }
			</style>
		</head>
		<body>
			<div id="header">
                <div class="version">
                    <span style="font-size: 12px; margin-left: 25px;">Version {{ version }}</span>
                </div>
                <div class="program-name">Wasp-M</div>
                <div class="reboot">
                    <input type="button" value="Update" onclick="update();" />
                    <input type="button" value="Reboot" onclick="reboot();" />
                </div>
            </div>
			<div id="content">
                <div id="left-column">
                    <table style="font-size: 14px;">
                      <tr>
                          <td valign="top" style="width: 130px; padding: 25px;">
                              <div style="height: 25px; line-height: 25px;"><a href="/index">Home</a></div>
                              <div style="height: 25px; line-height: 25px;"><a href="/config?type=system">Config</a></div>
                              <div style="height: 25px; line-height: 25px;"><a href="/data">Data</a></div>
                          </td>
                      </tr>
                  </table>
                </div>
                <div id="right-column">
				    {{ bodyContent }}
                </div>
			</div>
			<script>
				if (window.history.replaceState) {
					window.history.replaceState(null, null, window.location.href);
				}

				function update() {
					location.href = "http://" + window.location.host + "/update";
				}

				function reboot() {
					location.href = "http://" + window.location.host + "/reboot";
				}
			</script>
		</body>
	</html>
)";