# CMakeList.txt : CMake project for Wasp
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Wasp")

# Project version
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

# Execute git command to get the commit hash
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Generate version header
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/version.h"
    "#ifndef PROJECT_VERSION_H\n"
    "#define PROJECT_VERSION_H\n\n"
    "constexpr int PROJECT_VERSION_MAJOR = ${PROJECT_VERSION_MAJOR};\n"
    "constexpr int PROJECT_VERSION_MINOR = ${PROJECT_VERSION_MINOR};\n"
    "constexpr int PROJECT_VERSION_PATCH = ${PROJECT_VERSION_PATCH};\n\n"
    "constexpr auto PROJECT_GIT_COMMIT_HASH = \"${GIT_COMMIT_HASH}\";\n\n"
    "#endif // PROJECT_VERSION_H\n"
)

# CivetWeb needs this flag to use OpenSSL
add_definitions(-DOPENSSL_API_1_1)

# Add source to this project's executable.
add_executable (Wasp 
    "main.cpp" 
    "utilities/json_file_utility.hpp" 
    "files/settings.h" 
    "files/build.h" 
    "wasp.h" 
    "wasp.cpp"
    "gps/ublox.h" 
    "gps/ublox.cpp" 
    "gps/ublox_info.h"
    "imu/inertial_labs.h" 
    "imu/inertial_labs.cpp" 
    "utilities/serial_client.cpp" 
    "utilities/serial_client.h" 
    "utilities/udp_client.h" 
    "utilities/udp_client.cpp" 
    "utilities/pwm_interface.h"  
    "utilities/pwm_interface.cpp"
    "files/configuration.h" 
    "utilities/log_client.h" 
    "utilities/log_client.cpp"
    "managers/signal_manager.h" 
    "managers/signal_manager.cpp" 
    "utilities/web_server.h" 
    "utilities/web_server.cpp"
    "utilities/tcp_client.h" 
    "utilities/tcp_client.cpp" 
    "imu/imu_manager.h" 
    "imu/imu_manager.cpp" 
    "imu/imu_type.h" 
    "gps/gps_manager.h" 
    "gps/gps_manager.cpp" 
    "gps/gps_type.h" 
    "gps/novatel.h" 
    "gps/novatel.cpp" 
    "utilities/cot_utility.h" 
    "utilities/cot_utility.cpp"  
    "utilities/cot_info.h"
    "external/pugixml/pugiconfig.hpp"
    "external/pugixml/pugixml.cpp"
    "external/pugixml/pugixml.hpp"
    "external/civetweb/civetweb.c"
    "external/civetweb/civetweb.h"
    "external/inja/inja.hpp"
    "external/inja/utils.hpp"
    "utilities/constants.h" 
    "gps/atacnav.h" 
    "gps/atacnav.cpp" 
    "gps/atacnav_info.h"
    "web_pages/layout_page.h" 
    "web_pages/web_pages.h"  
    "web_pages/config_page.h" 
    "web_pages/reboot_page.h"
    "web_pages/login_page.h" 
    "web_pages/dev_page.h" 
    "web_pages/index_page.h"

 "web_pages/error_page.h" "web_pages/update_page.h" "web_pages/data_page.h")

# Include the generated header file directory
target_include_directories(Wasp PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Create a test files directory for ease of access and pass it as a compile definition
set(TEST_FILES_DIR "${CMAKE_SOURCE_DIR}/test_files")
target_compile_definitions(Wasp PRIVATE TEST_FILES_DIR="${TEST_FILES_DIR}")

# Create a web_files directory for ease of access and pass it as a compile definition
set(WEB_FILES_DIR "${CMAKE_SOURCE_DIR}/web_files")
target_compile_definitions(Wasp PRIVATE WEB_FILES_DIR="${WEB_FILES_DIR}")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Wasp PROPERTY CXX_STANDARD 20)
endif()

# End