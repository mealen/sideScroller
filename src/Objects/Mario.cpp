//
// Created by engin on 29.10.2017.
//

#include "Mario.h"

const int Mario::MOVE_SPEED = 4;
const int Mario::JUMP_SPEED = 16;

Mario::Mario(SDL_Rect mapPosition, SDL_Renderer *ren, int screenWidth, int &error)
        : screenWidth(screenWidth)
    {
        collisionBox = new AABB(
                mapPosition.x * TILE_SIZE,
                mapPosition.x * TILE_SIZE + TILE_SIZE -1,
                mapPosition.y * TILE_SIZE,
                mapPosition.y * TILE_SIZE + TILE_SIZE -1); //-1 because 32 is not part of box. pixels 0 - TILE_SIZE, TILE_SIZE excluded

    collisionBox->setPhysicsState(AABB::PhysicsState::DYNAMIC);
    std::string marioImage = Utils::getResourcePath("mario") + "mario.bmp";
    textures[STAND].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_move0.bmp";
    textures[MOVE].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_move1.bmp";
    textures[MOVE].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_move2.bmp";
    textures[MOVE].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_jump.bmp";
    textures[JUMP].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_death.bmp";
    textures[DEAD].push_back(Utils::loadTexture(ren, marioImage));

    if (textures[STAND][0] == nullptr ||
        textures[MOVE][0] == nullptr ||
        textures[MOVE][1] == nullptr ||
        textures[MOVE][2] == nullptr ||
        textures[JUMP][0] == nullptr ) {
        std::cerr << "Error loading Mario textures" << std::endl;
        error = 1;
        return;
    }

    error = 0;
}