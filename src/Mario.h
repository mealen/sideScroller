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

class Mario {
public:
    enum TextureNames {
        STAND, MOVE
    };

private:
    std::map<TextureNames, std::vector<SDL_Texture *>> textures;
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

    SDL_Texture *getTexture(TextureNames requiredTexture, long time) {
        switch (requiredTexture) {
            case STAND:
                return textures[STAND][0];
            case MOVE:
                return textures[MOVE][(time / 75) % 3];
            default:
                std::cerr << "Requested Texture type not found" << std::endl;
                exit(-1);
        }
    }

    const AABB* getPositionRect() const {
        return collitionBox;
    }

    void move(bool left, bool right, bool jump, bool crouch) {
        if(jump) {
            collitionBox->jump(jumpSpeed);
        }
        if (left) {
            if (collitionBox->getLeftBorder() + (320) > collitionBox->getMaxRight()) {
                Map::TileTypes tile = collitionBox->collide(-1 * moveSpeed, 0);
                if (tile == Map::PLAYER || tile == Map::EMPTY) {
                    collitionBox->moveLeft(moveSpeed);
                }
            } else {
                //std::cout <<"max right was " << collitionBox.maxRight << ", " << " while x1 was" << collitionBox.getLeftBorder() << std::endl;
            }
        }
        if (right) {
            Map::TileTypes tile = collitionBox->collide(moveSpeed, 0);
            if(tile == Map::PLAYER || tile == Map::EMPTY) {
                collitionBox->moveRight(moveSpeed);
            } else {
                std::cout << "right movement not possible because the tile is " << tile << std::endl;
            }
        }
        collitionBox->step();
    }

};


#endif //MARIO_MARIO_H
