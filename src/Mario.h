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

#include "Utils.h"
#include "Map.h"

class AABB {
public:
    int x1,y1, x2,y2;
    int representationFactor = 32;//this is pixel/tile ratio
    int maxright = 0;

    /**
     * Checks if the box is at collition with given tile coordinates.
     * The coordinates are not graphical, but logical.
     *
     * @param x
     * @param y
     * @return true if collides
     */
    Map::TileTypes collide(Map map) {
        if(x1 <0 ||y1 < 0) {
            std::cout << "exiting collision check for " << x1 << ", " << y1 << std::endl;
            return map.GROUND;
        }
        std::cout << "Checking " << x2/32 << ", " << y1/32 << ": " << map.getTileObject(x1/32, y1/32) << std::endl;
        return map.getTileObject(x2/32, y1/32);

    }

    bool moveRight() {
        x1 += 2;
        x2 += 2;
        maxright = std::max(x2, maxright);
    }

    bool moveLeft() {
        x1 -= 2;
        x2 -= 2;
    }
};

class Mario {
public:
    enum TextureNames {
        STAND, MOVE
    };

private:
    std::map<TextureNames, std::vector<SDL_Texture *>> textures;
    SDL_Rect graphicalPosition;
    int screenWidth;
    Map currentMap;
    AABB collitionBox;

public:

    Mario(SDL_Rect mapPosition, SDL_Renderer *ren, int screenWidth, Map currentMap, int &error);

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
        }
    }

    AABB getPositionRect() const {
        return collitionBox;
    }

    SDL_Rect move(bool left, bool right, bool jump, bool crouch) {
        if (left) {
            if (collitionBox.x1 + (320) > collitionBox.maxright) {
                collitionBox.moveLeft();
                Map::TileTypes tile = collitionBox.collide(currentMap);
                if (tile == Map::MARIO || tile == Map::EMPTY) {
                    graphicalPosition.x -= 2;
                    if (graphicalPosition.x < 0) {
                        graphicalPosition.x = 0;
                    }
                } else {
                    std::cout << "some collition " << tile << std::endl;
                    collitionBox.moveRight();
                }

            } else {
                std::cout <<"max right was " << collitionBox.maxright << ", " << " while x1 was" << collitionBox.x1 << std::endl;
            }
        }
        if (right) {
            collitionBox.moveRight();//FIXME this updates max right
            Map::TileTypes tile = collitionBox.collide(currentMap);
            if(tile == Map::MARIO || tile == Map::EMPTY) {
                graphicalPosition.x += 2;
                if (graphicalPosition.x + graphicalPosition.w > screenWidth / 2) {
                    graphicalPosition.x = screenWidth / 2 - graphicalPosition.w;
                }
            } else {
                std::cout << "some collition "<< tile << std::endl;
                collitionBox.moveLeft();
            }
        }
    }

};


#endif //MARIO_MARIO_H
