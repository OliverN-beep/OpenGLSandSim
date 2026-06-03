cmake_minimum_required(VERSION 3.20)

project(OpenGLSandSim)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML CONFIG REQUIRED COMPONENTS Graphics Window System)
find_package(nlohmann_json CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(OpenGLSandSim ${SOURCES})

target_link_libraries(OpenGLSandSim PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    nlohmann_json::nlohmann_json
)

target_include_directories(OpenGLSandSim PRIVATE
    src
)