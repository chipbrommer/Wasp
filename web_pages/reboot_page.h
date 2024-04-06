#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            reboot_page.h
// @brief           reboot html content in a std::string
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

const std::string rebootPage = R"(
	<div style="padding: 15px;">
		<h5>System Rebooting</h5>
	</div>
	<script>
		setTimeout(() => {
			location.href = "http://" + window.location.host + "/config?type=system";
		}, 10000);
	</script>
)";