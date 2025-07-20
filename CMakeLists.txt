cmake_minimum_required(VERSION 3.10)        # minimum required CMake version 

project(SignalRenderer LANGUAGES CXX)       # project name, specification of C++ language

set(CMAKE_CXX_STANDARD 14)                  # version of C++
set(CMAKE_CXX_STANDARD_REQUIRED ON)         # version required, error if unsupported by compiler

find_package(Qt5 REQUIRED COMPONENTS Widgets)        # Qt5 widgets module

add_executable(SignalRenderer signalRenderer.cpp SignalWidget.cpp)    # executable, source file(s)

target_link_libraries(SignalRenderer Qt5::Widgets)   # link Qt5 widget library to target

# Google Test Integration
include(FetchContent)            # download/build GoogleTest automatically

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()       # enable cmake's testing functionality

add_executable(SignalRendererTests test_main.cpp SignalWidget.cpp)    # test executable, source file(s)

target_link_libraries(SignalRendererTests gtest_main Qt5::Widgets)    # link GoogleTest and Qt5 Widgets to test target

# enable automatic test discovery with CTest using GoogleTest module
# include(GoogleTest)
# gtest_discover_tests(SignalRendererTests) 