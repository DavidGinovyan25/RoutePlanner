add_library(s STATIC
    RouteCache.cpp
)

target_include_directories(s PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)