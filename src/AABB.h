//
// Created by engin on 5.11.2017.
//

#ifndef MARIO_AABB_H
#define MARIO_AABB_H

#include "Map.h"

class AABB {
    int x1,y1, x2,y2;
    Map* map;
    int maxRight = 0;
    bool hasJumped = false;
    int upwardSpeed = 0;
public:

    /**
     *
     * Checks if the box is at collition with given tile coordinates.
     * The coordinates are not graphical, but logical.
     *
     * @param map
     * @param rightSpeed 1 for right, -1 for left, 0 for stopped
     * @param downSpeed 1 for up, -1 for down, 0 for stopped
     * @return true if collides
     */
    Map::TileTypes collide(int rightSpeed, int downSpeed) {
        Map::TileTypes tile = Map::EMPTY;
        //we need 4 checks, since at any given time, object can be at 4 different places.

        tile = std::max(tile, map->getTileObject((x1 + rightSpeed)/32, (y1 + downSpeed)/32));
        tile = std::max(tile, map->getTileObject((x1 + rightSpeed)/32, (y2 + downSpeed)/32));
        tile = std::max(tile, map->getTileObject((x2 + rightSpeed)/32, (y1 + downSpeed)/32));
        tile = std::max(tile, map->getTileObject((x2 + rightSpeed)/32, (y2 + downSpeed)/32));
        return tile;
    }

    void moveRight(int amount) {
        x1 += amount;
        x2 += amount;
        maxRight = std::max(x2, maxRight);
    }

    void moveLeft(int amount) {
        x1 -= amount;
        x2 -= amount;
    }

    void step() {
        Map::TileTypes tile = collide(0, -1 * upwardSpeed);
        //check if moving with upward speed is possible.
        if(tile != Map::EMPTY) {//if not possible, match the tile, and then stop
            y1 -= upwardSpeed;
            if(upwardSpeed > 0) {
                y1 = ((y1 + TILE_SIZE) / TILE_SIZE) * TILE_SIZE;
            } else {
                y1 = ((y1) / TILE_SIZE) * TILE_SIZE;
            }
            y2 = y1 + TILE_SIZE -1;
            upwardSpeed = 0;
            hasJumped = false;
        } else { //if possible update
            y1 -= upwardSpeed;
            y2 -= upwardSpeed;
            upwardSpeed -= 1;
        }
    }

    void jump(int amount) {
        if(collide(0, 1) != Map::EMPTY) {//jump only if you are stepping on something
            upwardSpeed = amount;
            hasJumped = true;
        }
    }

    int getLeftBorder() const {
        return x1;
    }

    int getRightBorder() const {
        return x2;
    }

    int getUpBorder() const {
        return y1;
    }

    int getDownBorder() const {
        return y2;
    }

    int getMaxRight() const {
        return maxRight;
    }

    bool isHasJumped() const {
        return hasJumped;
    }


    AABB(int left, int right, int up, int down, Map* map): map(map) {
        x1 = left;
        x2 = right;
        y1 = up;
        y2 = down;
        maxRight = x2;
    }
};

#endif //MARIO_AABB_H
