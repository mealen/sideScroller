//
// Created by engin on 29.10.2017.
//

#include "Mario.h"


Mario::Mario(SDL_Rect mapPosition, SDL_Renderer *ren, int screenWidth, Map currentMap, int &error)
        : graphicalPosition(mapPosition),
          screenWidth(screenWidth), currentMap(currentMap)
    {



    graphicalPosition.w = 32;
    graphicalPosition.h = 32;
    graphicalPosition.x *= 32;
    graphicalPosition.y *= 32;

    collitionBox.x1 = graphicalPosition.x;
    collitionBox.x2 = graphicalPosition.x + collitionBox.representationFactor;
    collitionBox.y1 = graphicalPosition.y;
    collitionBox.y2 = graphicalPosition.y + collitionBox.representationFactor;

    std::cout << graphicalPosition.x << ", " << graphicalPosition.y << std::endl;

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