cmake_minimum_required(VERSION 3.0)

project(rut_example VERSION 1.0.0 LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(rut_example "${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp")
target_link_libraries(rut_example PRIVATE rut)