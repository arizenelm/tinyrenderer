cmake_minimum_required(VERSION 3.8)

project(render)

set (SOURCES main.cpp model.cpp)

add_subdirectory(tgaimage)
include_directories(tgaimage)

add_executable(main ${SOURCES})
set_target_properties(main PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
target_link_libraries(main tgaimage)