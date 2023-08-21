cmake_minimum_required(VERSION 3.6)

project(MY_APP)

find_library(SDL3 SDL3)

add_library(main SHARED)

target_include_directories(main PRIVATE ../SDL/include/SDL3 ../SDL/include)

target_sources(main PRIVATE main.cpp)

target_link_libraries(main PRIVATE SDL3::SDL3)
