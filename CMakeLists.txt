cmake_minimum_required(VERSION 3.18)

project(Marble LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT_DIR ${CMAKE_SOURCE_DIR}/src)


add_subdirectory(src)