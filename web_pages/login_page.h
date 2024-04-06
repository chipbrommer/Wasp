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
    <form action="/login" method="post">
        Password: <input type="password" name="password" maxlength="99"><br>
        <input type="submit" value="Submit">
    </form>
)";
