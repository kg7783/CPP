cmake_minimum_required(VERSION 3.5.0)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")


#------------ Download CmakeRC Resource compiler ----------------#
file(DOWNLOAD "https://raw.githubusercontent.com/vector-of-bool/cmrc/master/CMakeRC.cmake"
                 "${CMAKE_BINARY_DIR}/CMakeRC.cmake")
include("${CMAKE_BINARY_DIR}/CMakeRC.cmake")

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
    )

project(CMakeFirstSteps VERSION 0.1.0 LANGUAGES C CXX)

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

find_package(Boost 1.84 REQUIRED COMPONENTS
                date_time 
                filesystem 
                iostreams 
                locale
                chrono)


find_package(fmt CONFIG REQUIRED)

#include(CTest)
#enable_testing()

file(GLOB SOURCES "src/*.cpp" "src/SDL/*.cpp")

add_executable(CMakeFirstSteps ${SOURCES})

target_link_libraries(CMakeFirstSteps 
                        ${SDL2_LIBRARIES} 
                        ${SDL2_IMAGE_LIBRARIES}
                        ${SDL2_MIXER_LIBRARIES}
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


