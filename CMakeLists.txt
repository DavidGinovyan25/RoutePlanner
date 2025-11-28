cmake_minimum_required(VERSION 3.10)
project(RouteCach)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_MINIMUM_REQUIRED TRUE)


include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(bin)
add_subdirectory(src)