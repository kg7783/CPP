cmake_minimum_required(VERSION 3.0.0)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")

project(CMakeFirstSteps VERSION 0.1.0 LANGUAGES C CXX)


find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

include(CTest)
enable_testing()

file(GLOB SOURCES "src/*.cpp")
add_executable(CMakeFirstSteps ${SOURCES})

target_link_libraries(CMakeFirstSteps ${SDL2_LIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


