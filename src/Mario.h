//
// Created by engin on 29.10.2017.
//

#ifndef MARIO_MARIO_H
#define MARIO_MARIO_H

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

#include <SDL2/SDL.h>
#include <SDL_rect.h>

#include "Constants.h"
#include "Utils.h"
#include "Map.h"
#include "AABB.h"
#include "DynamicObject.h"

class Mario : DynamicObject {

public:
    enum TextureNames {
        STAND, MOVE, JUMP
    };

private:
    std::map<TextureNames, std::vector<SDL_Texture *>> textures;
    TextureNames currentState = STAND;
    int screenWidth;
    Map* currentMap;
    AABB* collitionBox;

public:
    const int moveSpeed = 4;
    const int jumpSpeed = 16;
    Mario(SDL_Rect mapPosition, SDL_Renderer *ren, int screenWidth, Map* currentMap, int &error);

    ~Mario() {
        SDL_DestroyTexture(textures[STAND][0]);
        SDL_DestroyTexture(textures[MOVE][0]);
        SDL_DestroyTexture(textures[MOVE][1]);
        SDL_DestroyTexture(textures[MOVE][2]);
    }

    SDL_Texture *getTexture(long time) const {
        if(collitionBox->isHasJumped()) {
            return textures.at(JUMP).at(0);
        }
        switch (currentState) {
            case STAND:
                return textures.at(STAND).at(0);
            case MOVE:
                return textures.at(MOVE).at((time / 75) % 3);
            default:
                std::cerr << "Requested Texture type not found" << std::endl;
                exit(-1);
        }
    }

    const AABB* getPosition() const {
        return collitionBox;
    }

    void move(bool left, bool right, bool jump, bool crouch) {
        if(jump) {
            collitionBox->jump(jumpSpeed);
        }
        if (left) {
            currentState = MOVE;
            if (collitionBox->getLeftBorder() + (320) > collitionBox->getMaxRight()) {
                Map::TileTypes tile = collitionBox->collide(-1 * moveSpeed, 0);
                if (tile == Map::PLAYER || tile == Map::EMPTY) {
                    collitionBox->moveLeft(moveSpeed);
                }
            }
        }
        if (right) {
            currentState = MOVE;
            Map::TileTypes tile = collitionBox->collide(moveSpeed, 0);
            if(tile == Map::PLAYER || tile == Map::EMPTY) {
                collitionBox->moveRight(moveSpeed);
            }
        }
        if(!left && !right) {
            currentState = STAND;
        }
        collitionBox->step();
    }

};


#endif //MARIO_MARIO_H
