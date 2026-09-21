cmake_minimum_required(VERSION 3.16)
project(ponglab CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Connect SFML subdirectory
add_subdirectory(lib/SFML)

# Create the executable target
add_executable(ponglab pong_main.cpp)

# Provide the include directories for SFML
target_include_directories(ponglab PRIVATE lib/SFML/include)

# Link with SFML libraries
target_link_libraries(ponglab PRIVATE sfml-graphics sfml-window sfml-system)