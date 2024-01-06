cmake_minimum_required(VERSION 3.0.0)
project(CMakeStarterCpp VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()


#add_executable(CMakeStarterCpp src/main.cpp src/calc.cpp src/calc.h)

file (GLOB_RECURSE CMakeStarterCpp_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file (GLOB_RECURSE CMakeStarterCpp_HEADERS CONFIGURE_DEPENDS "src/*.h")

set (CMakeStarterCpp_INCLUDE_DIRS "")
foreach (_headerFile ${CMakeStarterCpp_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND CMakeStarterCpp_INCLUDE_DIRS ${_dir})
endforeach()
list (REMOVE_DUPLICATES CMakeStarterCpp_INCLUDE_DIRS)

add_executable(CMakeStarterCpp ${CMakeStarterCpp_SOURCES})
target_include_directories(CMakeStarterCpp PRIVATE ${CMakeStarterCpp_INCLUDE_DIRS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
