cmake_minimum_required(VERSION 3.8)
project(mario)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mconsole -Wall")
set(SOURCE_FILES src/main.cpp src/Mario.cpp src/Mario.h src/Utils.h src/Map.cpp src/Map.h src/Constants.h src/AABB.cpp src/AABB.h src/Brick.h src/World.cpp src/World.h src/DynamicObject.h)
add_executable(mario ${SOURCE_FILES})


INCLUDE(FindPkgConfig)
find_package(SDL2 REQUIRED)

if (NOT SDL2_FOUND)
    message(ERROR " SDL2 not found!")
endif (NOT SDL2_FOUND)

include_directories(${SDL2_INCLUDE_DIR})


find_package(SDL2_Mixer REQUIRED)

if (NOT SDL_MIXER_FOUND)
    message(ERROR " SDL_MIXER not found!")
endif (NOT SDL_MIXER_FOUND)

include_directories(${SDL_MIXER_INCLUDE_DIRS})

add_custom_target(copyData
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res)
add_dependencies(mario copyData)

TARGET_LINK_LIBRARIES(mario ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARY})
