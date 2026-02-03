cmake_minimum_required(VERSION 3.10)
project(RoutePlanner)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cpr REQUIRED)

add_subdirectory(lib)
add_subdirectory(bin)