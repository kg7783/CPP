cmake_minimum_required(VERSION 3.5.0)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")

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
                        )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


