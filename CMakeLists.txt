cmake_minimum_required(VERSION 3.18)

project(Marble LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT_DIR ${CMAKE_SOURCE_DIR}/src)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(FetchGoogleTest)
enable_testing()

add_subdirectory(src)
add_subdirectory(tests)