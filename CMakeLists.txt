cmake_minimum_required(VERSION 2.8)

project(TestEngine)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")

add_subdirectory(tinyxml)

file(GLOB SOURCES src/*.cpp)

find_package(SDL2)
find_package(SDL2_image)
find_package(SDL2_mixer)
find_package(SDL2_ttf)
find_package(OpenGL)
find_package(GLEW)

set(GL_LIBS ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
set(SDL2_LIBS ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY} ${SDL2MIXER_LIBRARY} ${SDL2TTF_LIBRARY})

set(GL_INC ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})
set(SDL2_INC ${SDL2_INCLUDE_DIR} ${SDLMIXER_INCLUDE_DIR} ${SDL2IMAGE_INCLUDE_DIR} ${SDL2TTF_INCLUDE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include tinyxml ${GL_INC} ${SDL2_INC})

add_executable(engineTest ${SOURCES})
target_link_libraries(engineTest LINK_PUBLIC tinyxml ${GL_LIBS} ${SDL2_LIBS})
