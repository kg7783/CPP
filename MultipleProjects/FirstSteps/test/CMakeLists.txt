cmake_minimum_required(VERSION 3.5.0)

#set(This ExampleTests)

#set(Sources
#    ExampleTests.cpp
#)

file(GLOB SOURCES "*.cpp")

add_executable(ExampleTests ${SOURCES})

target_link_libraries(ExampleTests PUBLIC
    gtest_main
    gmock_main
    CMakeFirstSteps
)

add_test(
    NAME ExampleTests
    COMMAND ExampleTests
)

