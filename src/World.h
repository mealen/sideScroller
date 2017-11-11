//
// Created by engin on 11.11.2017.
//

#ifndef MARIO_WORLD_H
#define MARIO_WORLD_H


#include "InteractiveObject.h"

class World {
    std::vector<InteractiveObject*> objects;
    Map* map;

public:
    void addObject(InteractiveObject* object) {
        objects.push_back(object);
    }

    /**
     *
     * @param ren  renderer
     * @param x -> current world x
     * @param y -> current world y
     * @param time -> get ticks
     */
    void render(SDL_Renderer *ren, int x, int y, long time) {
        for (unsigned int i = 0; i < objects.size(); ++i) {
            const AABB* pos = objects[i]->getPosition();
            SDL_Rect screenPos;
            screenPos.x = pos->getLeftBorder() - x;
            screenPos.y = pos->getUpBorder() - y;
            screenPos.w = TILE_SIZE;
            screenPos.h = TILE_SIZE;
            SDL_RenderCopyEx(ren, objects[i]->getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);
        }
    }

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
    Map::TileTypes collide(int rightSpeed, int downSpeed, InteractiveObject* interactiveObject) {
        Map::TileTypes tile = Map::EMPTY;
        //we need 4 checks, since at any given time, object can be at 4 different places.

        tile = std::max(tile, map->getTileObject((interactiveObject->getPosition()->getLeftBorder() + rightSpeed)/32, (interactiveObject->getPosition()->getUpBorder() + downSpeed)/32));
        tile = std::max(tile, map->getTileObject((interactiveObject->getPosition()->getLeftBorder() + rightSpeed)/32, (interactiveObject->getPosition()->getDownBorder() + downSpeed)/32));
        tile = std::max(tile, map->getTileObject((interactiveObject->getPosition()->getRightBorder() + rightSpeed)/32, (interactiveObject->getPosition()->getUpBorder() + downSpeed)/32));
        tile = std::max(tile, map->getTileObject((interactiveObject->getPosition()->getRightBorder() + rightSpeed)/32, (interactiveObject->getPosition()->getDownBorder() + downSpeed)/32));
        for (unsigned int i = 0; i < objects.size(); ++i) {
            if(interactiveObject->getPosition()->getUpBorder() + downSpeed> objects[i]->getPosition()->getDownBorder()) {
                continue;
            }

            if(interactiveObject->getPosition()->getDownBorder() + downSpeed< objects[i]->getPosition()->getUpBorder()) {
                continue;
            }

            if(interactiveObject->getPosition()->getRightBorder() + rightSpeed < objects[i]->getPosition()->getLeftBorder()) {
                continue;
            }

            if(interactiveObject->getPosition()->getLeftBorder() + rightSpeed > objects[i]->getPosition()->getRightBorder()) {
                continue;
            }
            tile = std::max(tile, objects[i]->getTileType());
        }
        return tile;
    }

    void step(InteractiveObject* interactiveObject) {
        AABB* aabb = interactiveObject->getPosition();

        int horizontalSpeed = aabb->getHorizontalSpeed();

        Map::TileTypes tile = collide(horizontalSpeed, 0, interactiveObject);

        if (tile == Map::EMPTY) {
            aabb->setLeftBorder(aabb->getLeftBorder() + horizontalSpeed);
            aabb->setRightBorder(aabb->getRightBorder() + horizontalSpeed);
        }

        aabb->setHorizontalSpeed(0);

        if (aabb->isHasJumpTriggered()) {
            aabb->setHasJumpTriggered(false);
            tile = collide(0, 1, interactiveObject);

            if (tile != Map::EMPTY) {
                aabb->setUpwardSpeed(aabb->getUpwardRequest());
            }
            aabb->setUpwardRequest(0);
        }
        int upwardSpeed = aabb->getUpwardSpeed();
        tile = collide(0, -1 * upwardSpeed, interactiveObject);
        //check if moving with upward speed is possible.
        if(tile != Map::EMPTY) {//if not possible, match the tile, and then stop
            aabb->setUpBorder(aabb->getUpBorder() - upwardSpeed);
            if(aabb->getUpwardSpeed() > 0) {
                aabb->setUpBorder(((aabb->getUpBorder() + TILE_SIZE) / TILE_SIZE) * TILE_SIZE);
            } else {
                aabb->setUpBorder((aabb->getUpBorder() / TILE_SIZE) * TILE_SIZE);
            }
            aabb->setDownBorder(aabb->getUpBorder() + TILE_SIZE -1);
            aabb->setUpwardSpeed(0);
            aabb->setHasJumped(false);
        } else { //if possible update
            aabb->setUpBorder(aabb->getUpBorder() - aabb->getUpwardSpeed());
            aabb->setDownBorder(aabb->getDownBorder() - aabb->getUpwardSpeed());
            aabb->setUpwardSpeed(aabb->getUpwardSpeed() - 1);
        }
    }

    World(Map* map) {
        this->map = map;
    }

};


#endif //MARIO_WORLD_H


