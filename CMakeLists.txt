cmake_minimum_required(VERSION 3.16)
project(space_invaders CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Connect SFML subdirectory
add_subdirectory(lib/SFML)

# Create the executable target
add_executable(space_invaders invaders_main.cpp)

# Provide the include directories for SFML
target_include_directories(space_invaders PRIVATE lib/SFML/include)

set_target_properties(space_invaders
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY
    "$<TARGET_FILE_DIR:space_invaders>"
)

# Copy res/ next to the exe after every build
add_custom_command(TARGET space_invaders POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/res"
    "$<TARGET_FILE_DIR:space_invaders>/res"
)
# Link with SFML libraries
target_link_libraries(space_invaders PRIVATE sfml-graphics sfml-window sfml-system)

