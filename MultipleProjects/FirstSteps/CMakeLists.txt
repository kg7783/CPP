cmake_minimum_required(VERSION 3.5.0)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")

project(CMakeFirstSteps VERSION 0.1.0 LANGUAGES C CXX)

find_package(box2d CONFIG REQUIRED)
include_directories (${Box2D_SOURCE_DIR})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(Boost 1.84 REQUIRED COMPONENTS
                date_time 
                filesystem 
                iostreams 
                locale
                chrono)


find_package(fmt CONFIG REQUIRED)

include(CTest)
enable_testing()

file(GLOB SOURCES "src/*.cpp")
add_executable(CMakeFirstSteps ${SOURCES})

target_link_libraries(CMakeFirstSteps 
                        ${SDL2_LIBRARIES} 
                        box2d::box2d 
                        fmt::fmt
                        Boost::boost
                        Boost::date_time 
                        Boost::filesystem
                        Boost::iostreams
                        Boost::locale
                        Boost::chrono)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


