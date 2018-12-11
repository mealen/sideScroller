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
    textures[SMALL][STAND].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_move0.bmp";
    textures[SMALL][MOVE].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_move1.bmp";
    textures[SMALL][MOVE].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_move2.bmp";
    textures[SMALL][MOVE].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_jump.bmp";
    textures[SMALL][JUMP].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_death.bmp";
    textures[SMALL][DEAD].push_back(Utils::loadTexture(ren, marioImage));


    marioImage = Utils::getResourcePath("mario/big") + "mario.bmp";
    textures[BIG][STAND].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario/big") + "mario_move0.bmp";
    textures[BIG][MOVE].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario/big") + "mario_move1.bmp";
    textures[BIG][MOVE].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario/big") + "mario_move2.bmp";
    textures[BIG][MOVE].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario/big") + "mario_jump.bmp";
    textures[BIG][JUMP].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario/big") + "mario_death.bmp";
    textures[BIG][DEAD].push_back(Utils::loadTexture(ren, marioImage));

    if (textures[SMALL][STAND][0] == nullptr ||
        textures[SMALL][MOVE][0] == nullptr ||
        textures[SMALL][MOVE][1] == nullptr ||
        textures[SMALL][MOVE][2] == nullptr ||
        textures[SMALL][JUMP][0] == nullptr ||
        textures[BIG][STAND][0] == nullptr ||
        textures[BIG][MOVE][0] == nullptr ||
        textures[BIG][MOVE][1] == nullptr ||
        textures[BIG][MOVE][2] == nullptr ||
        textures[BIG][JUMP][0] == nullptr ) {
        std::cerr << "Error loading Mario textures" << std::endl;
        error = 1;
        return;
    }

    error = 0;
}

bool Mario::isKilled() const {
    return killed;
}
