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
				a:hover {
					text-decoration: underline;
				} 

				a {
					text-decoration: none;
				}
			</style>
		</head>
		<body style="margin: 0px; padding: 0px; font-family: Verdana, Helvetica, Arial, sans-serif; background-color: #000000; color: #32CD32;">
			<div style="border-bottom: 1px solid #32CD32;">
				<table style="width: 100%; min-width: 800px; margin: 6px;">
					<tr>
						<td style="padding-left: 50px; text-align: left;">
							<span>Wasp-M</span>
							<span style="font-size: 12px; margin-left: 25px;">Version {{ version }}</span>
						</td>
						<td style="text-align: right; padding-right: 50px;">
							<input type="button" value="Reboot" onclick="reboot();" />
						</td>
					</tr>
				</table>
			</div>
			<div style="padding: 15px; min-width: 800px; max-width: 1200px; margin: auto;">
				{{ bodyContent }}
			</div>
			<script>
				if (window.history.replaceState) {
					window.history.replaceState(null, null, window.location.href);
				}

				function reboot() {
					location.href = "http://" + window.location.host + "/reboot";
				}
			</script>
		</body>
	</html>
)";