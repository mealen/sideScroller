cmake_minimum_required(VERSION 3.8)
project(mario)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp)
add_executable(mario ${SOURCE_FILES})


INCLUDE(FindPkgConfig)
find_package(SDL2 REQUIRED)

if (NOT SDL2_FOUND)
    message(ERROR " SDL2 not found!")
endif (NOT SDL2_FOUND)

include_directories(${SDL2_INCLUDE_DIR})

add_custom_target(copyData
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res)
add_dependencies(mario copyData)

TARGET_LINK_LIBRARIES(mario ${SDL2_LIBRARY})
