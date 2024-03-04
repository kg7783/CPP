cmake_minimum_required(VERSION 3.0.0)
set (CMAKE_CXX_STANDARD 23)
project(CMakeFirstSteps VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

file(GLOB SOURCES "src/*.cpp")
add_executable(CMakeFirstSteps ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
