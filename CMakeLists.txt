# CMakeList.txt : CMake project for Wasp, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Wasp")

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
    "utilities/tcp_server.h" 
    "utilities/tcp_server.cpp" 
    "imu/imu_manager.h" 
    "imu/imu_manager.cpp" 
    "imu/imu_type.h" 
    "gps/gps_manager.h" 
    "gps/gps_manager.cpp" 
    "gps/gps_type.h" 
    "gps/novatel.h" 
    "gps/novatel.cpp" "files/constants.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Wasp PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
