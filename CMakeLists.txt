cmake_minimum_required(VERSION 3.22)

project(render)

set (SOURCES main.cpp model.cpp)
set (HEADERS model.h geometry.h)
set (CMAKE_COLOR_MAKEFILE ON)

add_subdirectory(tgaimage)
include_directories(tgaimage)

add_executable(main ${SOURCES} ${HEADERS})
set_target_properties(main PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED ON 
                                CXX_EXTENSIONS OFF)
if ((CMAKE_COMPILER_IS_GNUCXX) OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W4")
endif()
target_link_libraries(main tgaimage)