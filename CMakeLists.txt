cmake_minimum_required(VERSION 3.14)
project(point)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(geo_to_utm)
include_directories(geo_to_utm/datum)
include_directories(geo_to_utm/utm)
add_library(Point point.cc point.h)
target_link_libraries(Point datum utm)
add_executable(point test.cpp)
target_link_libraries(point Point)