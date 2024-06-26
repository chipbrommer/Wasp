#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            login_page.h
// @brief           login html content in a std::string
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

const std::string loginPage = R"(
<h2>Login</h2>
<div id="error-message"></div>
<form action="/login" method="post">
    <label for="password">Password:</label><br>
    <input type="password" id="password" name="password" maxlength="49"><br><br>
    <input type="submit" value="Submit">
</form>
)";
