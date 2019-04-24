cmake_minimum_required(VERSION 3.8)
project(mario)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)

IF(APPLE)
    set(CMAKE_APPLE_FLAGS "-I/usr/local/include")
ENDIF()

IF(WIN32)
    set(CMAKE_WIN32_FLAGS "-mconsole")
ENDIF()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wnon-virtual-dtor -pedantic ${CMAKE_APPLE_FLAGS} ${CMAKE_WIN32_FLAGS}")

set(SOURCE_FILES src/main.cpp src/Objects/Mario.cpp src/Objects/Mario.h src/Utils.h src/Constants.h src/AABB.cpp src/AABB.h src/Objects/Brick.h src/Objects/CoinBox.h src/World.cpp src/World.h src/Objects/InteractiveObject.h src/Objects/Coin.cpp src/Objects/Coin.h src/Context.cpp src/Context.h src/Objects/CoinBox.cpp src/Objects/Goomba.cpp src/Objects/MushroomBox.cpp src/Objects/Mushroom.cpp src/Objects/HiddenCoinBox.cpp src/Objects/HiddenCoinBox.h src/Objects/Flower.cpp src/Objects/Fireball.cpp src/Objects/Koopa.cpp src/Objects/BoxBase.cpp src/Objects/BoxBase.h src/Objects/Star.cpp src/Objects/StarBrick.h src/HUD.cpp src/Objects/AnimatedScore.cpp src/InputHandler.cpp src/InputHandler.h src/Objects/Flag.cpp src/Objects/Flag.h)
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

include_directories(${SDL_TTF_INCLUDE_DIRS})

find_package(SDL2_TTF REQUIRED)

if (NOT SDL_TTF_FOUND)
    message(ERROR " SDL_TTF not found!")
endif (NOT SDL_TTF_FOUND)

include_directories(${SDL_MIXER_INCLUDE_DIRS})

add_custom_target(copyData
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res)
add_dependencies(mario copyData)

TARGET_LINK_LIBRARIES(mario ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARY} ${SDL2_TTF_LIBRARY})
