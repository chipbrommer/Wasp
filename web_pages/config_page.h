#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            config_page.h
// @brief           config html content in a std::string
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

const std::string configPage = R"(
	<style type="text/css">
		a:hover {
			text-decoration: underline;
		} 

		a {
			text-decoration: none;
			color: aqua;
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
	</style>
	<table style="font-size: 14px;">
		<tr>
			<td valign="top" style="width: 130px; padding: 25px; border-right: 1px solid #32CD32;">
				<div style="height: 25px; line-height: 25px;"><a href="/config?type=system">System</a></div>
			</td>
			<td valign="top" style="padding: 25px;">
				<form method="post">
					<div>
						{{ configContent }}
					</div>
					<div>
						<input type="submit" value="Save" />
						{% if isSaveSuccessful == true and didSomeNotSave == false %}
						<span id="saveSuccessfulNotification" style="background-color: green; font-size: 12px; padding: 2px 6px; margin-left: 25px; border-radius: 4px;">
							save successful
						</span>
						{% else if isSaveSuccessful == true and didSomeNotSave == true and isSaveFailed == false %}
						<span id="saveSuccessfulNotification" style="background-color: yellow; font-size: 12px; padding: 2px 6px; margin-left: 25px; border-radius: 4px;">
							save partial
						</span>
						{% else if isSaveFailed == true %}
						<span id="saveSuccessfulNotification" style="background-color: red; font-size: 12px; padding: 2px 6px; margin-left: 25px; border-radius: 4px;">
							save failed
						</span>
						{% endif %}
					</div>
				</form>
			</td>
		</tr>
	</table>
)";