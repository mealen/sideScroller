//
// Created by engin on 29.10.2017.
//

#include "Mario.h"

Mario::Mario(SDL_Rect mapPosition, SDL_Renderer *ren, int screenWidth, int &error) : position(mapPosition),
screenWidth(screenWidth) {

    position.w = 32;
    position.h = 32;
    position.x *= 32;
    position.y *= 32;

    std::cout << position.x << ", " << position.y << std::endl;

    std::string marioImage = Utils::getResourcePath("mario") + "mario.bmp";
    textures[STAND].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_move0.bmp";
    textures[MOVE].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_move1.bmp";
    textures[MOVE].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_move2.bmp";
    textures[MOVE].push_back(Utils::loadTexture(ren, marioImage));

    if (textures[STAND][0] == nullptr ||
        textures[MOVE][0] == nullptr ||
        textures[MOVE][1] == nullptr ||
        textures[MOVE][2] == nullptr) {
        std::cerr << "Error loading Mario textures" << std::endl;
        error = 1;
        return;
    }

    error = 0;
}