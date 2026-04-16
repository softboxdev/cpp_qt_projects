cmake_minimum_required(VERSION 3.10)
project(MySecondQtProject)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# standart C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# FIND LIBS
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets)
