cmake_minimum_required(VERSION 3.0)
project(Game_Port)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compile commands export
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add compile options
add_compile_options(-g)
add_compile_options(-O2)

# Include header files
file(GLOB INCLUDE_SOURCES "include/*.hpp")
include_directories("include")

# Include raylib
include_directories("/home/aether/raylib/cmake")
set(raylib_DIR "/home/aether/raylib/cmake")
find_package(raylib REQUIRED)

# Add source files
file(GLOB SOURCE_FILES "src/*.cpp")

# Add executable
add_executable(Game_Port ${SOURCE_FILES} ${INCLUDE_SOURCES})

# Link libraries
target_link_libraries(Game_Port ${raylib_LIBRARIES})