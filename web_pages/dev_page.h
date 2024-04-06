#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            dev_page.h
// @brief           dev html content in a std::string
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

const std::string devPage = R"(
    <h4>Developer Access</h4>
    <p>This is the developer access page.</p>
    <form>
        <!-- TODO add form items here for serial ports -->
    </form>
)";