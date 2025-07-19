cmake_minimum_required(VERSION 3.10)        # minimum required CMake version 

project(SignalRenderer LANGUAGES CXX)       # project name, specification of C++ language

set(CMAKE_CXX_STANDARD 14)                  # version of C++
set(CMAKE_CXX_STANDARD_REQUIRED ON)         # version required, error if unsupported by compiler

find_package(Qt5 REQUIRED COMPONENTS Widgets)        # Qt5 widgets module

add_executable(SignalRenderer signalRenderer.cpp SignalWidget.cpp)    # executable, source file(s)

target_link_libraries(SignalRenderer Qt5::Widgets)   # link Qt5 widget library to target