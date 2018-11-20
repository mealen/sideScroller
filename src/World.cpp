//
// Created by engin on 11.11.2017.
//

#include "World.h"

#include "Context.h"

Map::TileTypes World::collide(int rightSpeed, int downSpeed, long time, std::shared_ptr<Context> context) {
    Map::TileTypes tile = Map::EMPTY;
    //we need 4 checks, since at any given time, object can be at 4 different places.
    tile = std::max(tile, map->getTileObject((context->getPlayer()->getPosition()->getLeftBorder() + rightSpeed)/32,  (context->getPlayer()->getPosition()->getUpBorder() + downSpeed)/32));
    tile = std::max(tile, map->getTileObject((context->getPlayer()->getPosition()->getLeftBorder() + rightSpeed)/32,  (context->getPlayer()->getPosition()->getDownBorder() + downSpeed)/32));
    tile = std::max(tile, map->getTileObject((context->getPlayer()->getPosition()->getRightBorder() + rightSpeed)/32, (context->getPlayer()->getPosition()->getUpBorder() + downSpeed)/32));
    tile = std::max(tile, map->getTileObject((context->getPlayer()->getPosition()->getRightBorder() + rightSpeed)/32, (context->getPlayer()->getPosition()->getDownBorder() + downSpeed)/32));

    // mario fall down from ground
    if (tile == Map::TileTypes::OUT_OF_MAP) {

    }


    InteractiveObject* collidingObject = NULL;
    int collisionSide = 0;//1 down, 2 up, 3 left 4 right
    for (unsigned int i = 0; i < objects.size(); ++i) {
        // that 4 if finds if that objects collide
        if(context->getPlayer()->getPosition()->getUpBorder() + downSpeed> objects[i]->getPosition()->getDownBorder()) {
            continue;
        }

        if(context->getPlayer()->getPosition()->getDownBorder() + downSpeed< objects[i]->getPosition()->getUpBorder()) {
            continue;
        }

        if(context->getPlayer()->getPosition()->getRightBorder() + rightSpeed < objects[i]->getPosition()->getLeftBorder()) {
            continue;
        }

        if(context->getPlayer()->getPosition()->getLeftBorder() + rightSpeed > objects[i]->getPosition()->getRightBorder()) {
            continue;
        }
        if(tile < objects[i]->getTileType()) {

            collidingObject = objects[i];
            //now we know there is a collition, check what is the direction of collision
            if(context->getPlayer()->getPosition()->getUpBorder() > objects[i]->getPosition()->getDownBorder()) {
                tile = objects[i]->getTileType();
                collisionSide = 1;
            }
            if(context->getPlayer()->getPosition()->getDownBorder() < objects[i]->getPosition()->getUpBorder()) {
                tile = objects[i]->getTileType();
                collisionSide = 2;
            }

            if(context->getPlayer()->getPosition()->getRightBorder() < objects[i]->getPosition()->getLeftBorder()) {
                tile = objects[i]->getTileType();
                collisionSide = 3;
            }

            if(context->getPlayer()->getPosition()->getLeftBorder() > objects[i]->getPosition()->getRightBorder()) {
                tile = objects[i]->getTileType();
                collisionSide = 4;
            }
        }
    }
    if(collidingObject != NULL) {
        collidingObject->interactWithSide(context, collisionSide, time);
    }
    return tile;
}

