cmake_minimum_required(VERSION 3.0)

project(Game_Port)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-g)
add_compile_options(-O2)

file(GLOB INCLUDE_SOURCES "include/*.cpp")

include_directories("/home/aether/raylib/cmake")
set(raylib_DIR "/home/aether/raylib/cmake")


find_package(raylib REQUIRED)

add_executable(Game_Port src/main.cpp ${INCLUDE_SOURCES})
target_link_libraries(Game_Port ${raylib_LIBRARIES})

