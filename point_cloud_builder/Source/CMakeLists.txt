cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(example_get_max_min_coordinates)

find_package(PCL 1.2 REQUIRED)

include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

add_executable (example_get_max_min_coordinates example_get_max_min_coordinates.cpp)
target_link_libraries (example_get_max_min_coordinates ${PCL_LIBRARIES})