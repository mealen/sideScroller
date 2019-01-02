//
// Created by engin on 11.11.2017.
//

#include "World.h"

#include "Context.h"

Map::TileTypes World::collide(int rightSpeed, int downSpeed, long time, std::shared_ptr<Context> context,
                              std::shared_ptr<InteractiveObject> interactiveObject) {
    Map::TileTypes tile = Map::EMPTY;
    //we need 4 checks, since at any given time, object can be at 4 different places.
    tile = std::max(tile, map->getTileObject((interactiveObject->getPosition()->getLeftBorder() + rightSpeed)/TILE_SIZE,  (interactiveObject->getPosition()->getUpBorder() + downSpeed)/TILE_SIZE));
    tile = std::max(tile, map->getTileObject((interactiveObject->getPosition()->getLeftBorder() + rightSpeed)/TILE_SIZE,  (interactiveObject->getPosition()->getDownBorder() + downSpeed)/TILE_SIZE));
    tile = std::max(tile, map->getTileObject((interactiveObject->getPosition()->getRightBorder() + rightSpeed)/TILE_SIZE, (interactiveObject->getPosition()->getUpBorder() + downSpeed)/TILE_SIZE));
    tile = std::max(tile, map->getTileObject((interactiveObject->getPosition()->getRightBorder() + rightSpeed)/TILE_SIZE, (interactiveObject->getPosition()->getDownBorder() + downSpeed)/TILE_SIZE));

    //this tile is the tile that is not interactive object
    std::shared_ptr<InteractiveObject> collidingObject = nullptr;
    std::shared_ptr<InteractiveObject> testingObject = nullptr;
    int collisionSide = 5;//1 down, 2 up, 3 left 4 right
    for (unsigned int i = 0; i < objects.size(); ++i) {
        if(objects[i] == interactiveObject) {
            continue;
        }
        // that 4 finds if that object collide with caller
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

        // collision are prioritized as following 1 = 2 > 3 = 4
        //this part makes sure colliding object has the highest priority by tile type
        if(tile <= objects[i]->getTileType()) {

            testingObject = objects[i];
            //now we know there is a collision, check what is the direction of collision
            if(interactiveObject->getPosition()->getUpBorder() >= objects[i]->getPosition()->getDownBorder()) {
                collidingObject = testingObject;
                tile = objects[i]->getTileType();
                collisionSide = 1;

            }
            if(interactiveObject->getPosition()->getDownBorder() <= objects[i]->getPosition()->getUpBorder()) {
                collidingObject = testingObject;
                tile = objects[i]->getTileType();
                collisionSide = 2;
            }

            if(interactiveObject->getPosition()->getRightBorder() <= objects[i]->getPosition()->getLeftBorder()) {
                if(collisionSide > 2 ) {
                    tile = objects[i]->getTileType();
                    collisionSide = 3;
                    collidingObject = testingObject;
                }
            }

            if(interactiveObject->getPosition()->getLeftBorder() >= objects[i]->getPosition()->getRightBorder()) {
                if(collisionSide > 2 ) {
                    tile = objects[i]->getTileType();
                    collisionSide = 4;
                    collidingObject = testingObject;
                }
            }
        }
    }

    if (collidingObject == interactiveObject) {
        return Map::EMPTY;
    }
    if(collidingObject != NULL ) {
        tile = collidingObject->interactWithSide(context, interactiveObject, collisionSide, time);
        int reverseCollisionSide;
        switch (collisionSide) {
            case 1: reverseCollisionSide = 2; break;
            case 2: reverseCollisionSide = 1; break;
            case 3: reverseCollisionSide = 4; break;
            case 4: reverseCollisionSide = 3; break;
        }
        interactiveObject->interactWithSide(context, collidingObject, reverseCollisionSide, time);
    } else {
        if(tile!= Map::EMPTY) {
            interactiveObject->collideWithSide(context, tile, -1, time);//FIXME -1 means unknown, this method should be removed and everything should be object
        }
    }
    return tile;
}

void World::stepSimulation(long time, std::shared_ptr<Context> context) {
    context->getPlayer()->step(time);
    int middleOfScreenPixel = SCREEN_WIDTH / 2;

    for (size_t i = 0; i < this->objects.size(); ++i) {
        if (context->getPlayer()->getPosition()->getMaxRight() + middleOfScreenPixel > this->objects[i]->getPosition()->getLeftBorder()) {
            this->objects[i]->step(time);
        }
    }

    for (size_t i = 0; i < this->objects.size(); ++i) {
        std::shared_ptr<InteractiveObject> interactiveObject = this->objects[i];
        stepSingleObject(time, context, interactiveObject);
    }

    for(unsigned int i = 0; i < objects.size(); i++) {
        if(objects[i]->waitingForDestroy()) {
            objects.erase(objects.begin() + i);
        }
    }
}

