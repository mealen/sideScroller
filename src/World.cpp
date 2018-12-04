//
// Created by engin on 11.11.2017.
//

#include "World.h"

#include "Context.h"

Map::TileTypes World::collide(int rightSpeed, int downSpeed, long time, std::shared_ptr<Context> context,
                              std::shared_ptr<InteractiveObject> interactiveObject) {
    Map::TileTypes tile = Map::EMPTY;
    //we need 4 checks, since at any given time, object can be at 4 different places.
    tile = std::max(tile, map->getTileObject((interactiveObject->getPosition()->getLeftBorder() + rightSpeed)/32,  (interactiveObject->getPosition()->getUpBorder() + downSpeed)/32));
    tile = std::max(tile, map->getTileObject((interactiveObject->getPosition()->getLeftBorder() + rightSpeed)/32,  (interactiveObject->getPosition()->getDownBorder() + downSpeed)/32));
    tile = std::max(tile, map->getTileObject((interactiveObject->getPosition()->getRightBorder() + rightSpeed)/32, (interactiveObject->getPosition()->getUpBorder() + downSpeed)/32));
    tile = std::max(tile, map->getTileObject((interactiveObject->getPosition()->getRightBorder() + rightSpeed)/32, (interactiveObject->getPosition()->getDownBorder() + downSpeed)/32));

    //this tile is the tile that is not interactive object

    std::shared_ptr<InteractiveObject> collidingObject = nullptr;
    int collisionSide = 0;//1 down, 2 up, 3 left 4 right
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

        //this part makes sure colliding object has the highest priority by tile type
        if(tile < objects[i]->getTileType()) {

            collidingObject = objects[i];
            //now we know there is a collision, check what is the direction of collision
            if(interactiveObject->getPosition()->getUpBorder() >= objects[i]->getPosition()->getDownBorder()) {
                tile = objects[i]->getTileType();
                collisionSide = 1;
            }
            if(interactiveObject->getPosition()->getDownBorder() <= objects[i]->getPosition()->getUpBorder()) {
                tile = objects[i]->getTileType();
                collisionSide = 2;
            }

            if(interactiveObject->getPosition()->getRightBorder() <= objects[i]->getPosition()->getLeftBorder()) {
                tile = objects[i]->getTileType();
                collisionSide = 3;
            }

            if(interactiveObject->getPosition()->getLeftBorder() >= objects[i]->getPosition()->getRightBorder()) {
                tile = objects[i]->getTileType();
                collisionSide = 4;
            }
        }
    }
    if(collidingObject != NULL ) {
        tile = collidingObject->interactWithSide(context, interactiveObject, collisionSide, time);
    } else {
        if(tile!= Map::EMPTY) {
            interactiveObject->collideWithSide(context, tile, -1, time);//FIXME -1 means unknown, this method should be removed and everything should be object
        }
    }
    return tile;
}

void World::stepSimulation(long time, std::shared_ptr<Context> context) {
    context->getPlayer()->step(time);

    for (size_t i = 0; i < this->objects.size(); ++i) {
        this->objects[i]->step(time);
    }
    stepSingleObject(time, context, this->mario);
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

