//
// Created by engin on 11.11.2017.
//

#ifndef MARIO_BRICK_H
#define MARIO_BRICK_H


#include <SDL_render.h>

#include "AABB.h"
#include "Utils.h"
#include "DynamicObject.h"

class Brick : public DynamicObject {
    SDL_Texture *texture;
    AABB* collisionBox;

public:
    Brick(SDL_Renderer *ren, int x, int y, Map* map) {//FIXME this should not need  renderer and map
        collisionBox = new AABB(
                x * TILE_SIZE,
                x * TILE_SIZE + TILE_SIZE -1,
                y * TILE_SIZE,
                y * TILE_SIZE + TILE_SIZE -1, map); //-1 because 32 is not part of box. pixels 0 - TILE_SIZE, TILE_SIZE excluded


        std::string marioImage = Utils::getResourcePath("brick") + "brickred.bmp";
        texture = Utils::loadTexture(ren, marioImage);
    }

    SDL_Texture* getTexture(long time) const {
        return texture;
    };

    const AABB* getPosition() const {
        return collisionBox;
    };


};


#endif //MARIO_BRICK_H
