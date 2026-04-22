cmake_minimum_required(VERSION 3.10)
project(SystemMonitor VERSION 1.0.0)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CSS_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Network Widgets Charts)

add_subdirectory(common)
add_subdirectory(agent)
add_subdirectory(gui)


add_custom_target(run-all
    COMMAND ${CMAKE_COMMAND} -E echo "Building all components..."
    DEPENDS common agent gui)

