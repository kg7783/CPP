cmake_minimum_required(VERSION 3.5.0)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")

#========== Set vcpkg path BEFORE project(...), otherwise error ==================#
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE} $ENV{HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake)

#========== Define project(...) ==================#
project(CMakeFirstSteps VERSION 0.1.0 LANGUAGES C CXX)

#========== Additional CMake modules: FirstSteps/src/cmake/ ==================#
# - CMakeRC.cmake
# - FindSDL2_image.cmake
# - FindSDL2_mixer.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/src/cmake)

#========== Print some variables for info ==================#
include(CMakePrintHelpers)
cmake_print_variables(PROJECT_SOURCE_DIR)
cmake_print_variables(CMAKE_BINARY_DIR)
cmake_print_variables(CMAKE_MODULE_PATH)
cmake_print_variables(CMAKE_CURRENT_SOURCE_DIR)
cmake_print_variables(CMAKE_TOOLCHAIN_FILE)

# ========== Already includes with 
# - set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/src/cmake)
# => so this is not necessary:

#------------ Download CmakeRC Resource compiler ----------------#
#file(DOWNLOAD "https://raw.githubusercontent.com/vector-of-bool/cmrc/master/CMakeRC.cmake"
#                 "${CMAKE_BINARY_DIR}/CMakeRC.cmake")
#include("${CMAKE_BINARY_DIR}/CMakeRC.cmake")

# Include the main module of CmakeRC Resource compiler
include(CMakeRC)

#========== Targets Configurations ============#
cmrc_add_resource_library(
    FirstSteps-resources

    ALIAS FirstSteps::rc
    NAMESPACE FirstSteps

    rc/_U1A1218.JPG

    src/SDL/rc/beat.wav
    src/SDL/rc/dot.bmp
    src/SDL/rc/high.wav
    src/SDL/rc/medium.wav
    src/SDL/rc/scratch.wav
    src/SDL/rc/x.bmp
    src/SDL/rc/bg_30.png
    src/SDL/rc/down.bmp
    src/SDL/rc/left.bmp
    src/SDL/rc/press.bmp
    src/SDL/rc/stretch.bmp
    src/SDL/rc/bg.png
    src/SDL/rc/loaded.png
    src/SDL/rc/prompt.png
    src/SDL/rc/texture.png
    src/SDL/rc/dot_27.bmp
    src/SDL/rc/hello_world.bmp
    src/SDL/rc/low.wav
    src/SDL/rc/right.bmp
    src/SDL/rc/up.bmp
    src/SDL/rc/viewport.png
    src/SDL/rc/foo.png
    src/SDL/rc/background.png
    src/SDL/rc/dots.png
    src/SDL/rc/colors.png
    src/SDL/rc/fadein.png
    src/SDL/rc/fadeout.png
    src/SDL/rc/foo_14.png
    src/SDL/rc/arrow.png
    src/SDL/rc/lazy.ttf
    src/SDL/rc/button.png
    src/SDL/rc/down.png
    src/SDL/rc/left.png
    src/SDL/rc/press.png
    src/SDL/rc/right.png
    src/SDL/rc/up.png
    src/SDL/rc/splash.png
    src/SDL/rc/dot_26.bmp
    src/SDL/rc/window.png
    src/SDL/rc/blue.bmp
    src/SDL/rc/green.bmp
    src/SDL/rc/red.bmp
    src/SDL/rc/shimmer.bmp
    src/SDL/rc/tiles.png
    src/SDL/rc/foo_40.png
    src/SDL/rc/lazy.map

    src/SDL/rc/lazyfont.png
    src/SDL/rc/foo_walk_0.png
    src/SDL/rc/foo_walk_1.png
    src/SDL/rc/foo_walk_2.png
    src/SDL/rc/foo_walk_3.png

    src/SDL/rc/splash_45.png
    src/SDL/rc/splash_46.png
    src/SDL/rc/splash_47.png
    src/SDL/rc/splash_48.png
    src/SDL/rc/splash_49.png
    )

# Add box2d library
find_package(box2d CONFIG REQUIRED)
include_directories (${Box2D_SOURCE_DIR})

# Add SDL2 library
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Add SDL2_image library
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

# Add SDL2_mixer library
find_package(SDL2_mixer REQUIRED)
include_directories(${SDL2_MIXER_INCLUDE_DIRS})

# Add SDL2_ttf library
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_TTF_INCLUDE_DIRS})

# Add GLEW library
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
#link_libraries(${GLEW_LIBRARIES})

# Add OpenGL library
set(OpenGL_GL_PREFERENCE "LEGACY")
#set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
#link_libraries(${OpenGL_LIBRARIES})

# Add Boost library
find_package(Boost 1.84 REQUIRED COMPONENTS
                date_time 
                filesystem 
                iostreams 
                locale
                chrono
            )

# Add fmt library
find_package(fmt CONFIG REQUIRED)

file(GLOB SOURCES "src/*.cpp" "src/SDL/*.cpp")

# Activate this for executing the unit test
option(RUN_UNIT_TESTS "Run Catch2 unit tests" ON)

if(RUN_UNIT_TESTS)
    enable_testing()
    include(CodeCoverage)
    append_coverage_compiler_flags()

    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

    add_subdirectory(googletest_lib)
    add_subdirectory(test)
    add_library(CMakeFirstSteps STATIC ${SOURCES})



    set(GCOVR_PATH "/usr/bin/gcovr")

    setup_target_for_coverage_gcovr_xml(
        NAME TestDoMathCoverageXml
        EXECUTABLE ExampleTests
        DEPENDENCIES ExampleTests CMakeFirstSteps
    )

    setup_target_for_coverage_gcovr_html(
        NAME TestDoMathCoverageHtml
        EXECUTABLE ExampleTests
        DEPENDENCIES ExampleTests CMakeFirstSteps
    )








else()
    add_executable(CMakeFirstSteps ${SOURCES})
endif()
   

target_link_libraries(CMakeFirstSteps 
                        ${SDL2_LIBRARIES} 
                        ${SDL2_IMAGE_LIBRARIES}
                        ${SDL2_MIXER_LIBRARIES}
                        ${SDL2_TTF_LIBRARIES}
                        box2d::box2d 
                        fmt::fmt
                        Boost::boost
                        Boost::date_time 
                        Boost::filesystem
                        Boost::iostreams
                        Boost::locale
                        Boost::chrono
                        OpenGL::GL
                        GLEW::GLEW
                        FirstSteps::rc
                        )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


