set(CMAKE_CXX_FLAGS "-DTIXML_USE_STL -Wall -O3")

file(GLOB TINYXML_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(tinyxml ${TINYXML_SRC})
target_include_directories(tinyxml PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
