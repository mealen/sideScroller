//
// Created by engin on 11.11.2017.
//

#include <fstream>
#include <iostream>

#include "World.h"
#include "Context.h"
#include "Constants.h"


TileTypes World::collide(int rightSpeed, int downSpeed, long time, std::shared_ptr<Context> context,
                              std::shared_ptr<InteractiveObject> interactiveObject) {
    TileTypes tile = TileTypes::EMPTY;
    //we need 4 checks, since at any given time, object can be at 4 different places.

    int horCheck, verCheck;
    CollisionSide collisionWithStaticSide = CollisionSide::INVALID;
    if(rightSpeed >= 0) {
        horCheck = interactiveObject->getPosition()->getRightBorder() + rightSpeed;
    } else {
        horCheck = interactiveObject->getPosition()->getLeftBorder() + rightSpeed;
    }

    if(downSpeed >0) {
        verCheck = interactiveObject->getPosition()->getDownBorder() + downSpeed;
    } else {
        verCheck = interactiveObject->getPosition()->getUpBorder() + downSpeed;
    }

    tile = getTileObject(horCheck/TILE_SIZE, verCheck/TILE_SIZE);
    if(tile != TileTypes::EMPTY) {
        if (getTileObject(horCheck / TILE_SIZE, (verCheck - downSpeed) / TILE_SIZE) != TileTypes::EMPTY) {
            if (rightSpeed >= 0) {
                collisionWithStaticSide = CollisionSide::RIGHT;
            } else {
                collisionWithStaticSide = CollisionSide::LEFT;
            }
        } else {
            if (downSpeed >= 0) {
                collisionWithStaticSide = CollisionSide::DOWN;
            } else {
                collisionWithStaticSide = CollisionSide::UP;
            }
        }
    }

    //this tile is the tile that is not interactive object
    std::shared_ptr<InteractiveObject> collidingObject = nullptr;
    std::shared_ptr<InteractiveObject> testingObject = nullptr;
    CollisionSide collisionSide = CollisionSide::INVALID;
    for (unsigned int i = 0; i < objects.size(); ++i) {
        if (objects[i] == interactiveObject) {
            continue;
        }
        // that 4 finds if that object collide with caller
        if (interactiveObject->getPosition()->getUpBorder() + downSpeed > objects[i]->getPosition()->getDownBorder()) {
            continue;
        }

        if (interactiveObject->getPosition()->getDownBorder() + downSpeed < objects[i]->getPosition()->getUpBorder()) {
            continue;
        }

        if (interactiveObject->getPosition()->getRightBorder() + rightSpeed <
            objects[i]->getPosition()->getLeftBorder()) {
            continue;
        }

        if (interactiveObject->getPosition()->getLeftBorder() + rightSpeed >
            objects[i]->getPosition()->getRightBorder()) {
            continue;
        }

        // collision are prioritized as following 1 = 2 > 3 = 4
        //this part makes sure colliding object has the highest priority by tile type
        if (tile <= objects[i]->getTileType()) {

            testingObject = objects[i];
            //now we know there is a collision, check what is the direction of collision
            if (interactiveObject->getPosition()->getUpBorder() >= objects[i]->getPosition()->getDownBorder()) {
                collidingObject = testingObject;
                tile = objects[i]->getTileType();
                collisionSide = CollisionSide::DOWN;

            }
            if (interactiveObject->getPosition()->getDownBorder() <= objects[i]->getPosition()->getUpBorder()) {
                collidingObject = testingObject;
                tile = objects[i]->getTileType();
                collisionSide = CollisionSide::UP;
            }

            if (interactiveObject->getPosition()->getRightBorder() <= objects[i]->getPosition()->getLeftBorder()) {
                if (collisionSide == CollisionSide::LEFT || collisionSide == CollisionSide::RIGHT || collisionSide == CollisionSide::INVALID) {
                    tile = objects[i]->getTileType();
                    collisionSide = CollisionSide::LEFT;
                    collidingObject = testingObject;
                }
            }

            if (interactiveObject->getPosition()->getLeftBorder() >= objects[i]->getPosition()->getRightBorder()) {
                if (collisionSide == CollisionSide::LEFT || collisionSide == CollisionSide::RIGHT || collisionSide == CollisionSide::INVALID) {
                    tile = objects[i]->getTileType();
                    collisionSide = CollisionSide::RIGHT;
                    collidingObject = testingObject;
                }
            }
        }
    }

    if (collidingObject == interactiveObject) {
        return TileTypes::EMPTY;
    }
    if (collidingObject != NULL) {
        tile = collidingObject->interactWithSide(context, interactiveObject, collisionSide, time);
        CollisionSide reverseCollisionSide;
        switch (collisionSide) {
            case CollisionSide::DOWN:
                reverseCollisionSide = CollisionSide::UP;
                break;
            case CollisionSide::UP:
                reverseCollisionSide = CollisionSide::DOWN;
                break;
            case CollisionSide::LEFT:
                reverseCollisionSide = CollisionSide::RIGHT;
                break;
            case CollisionSide::RIGHT:
                reverseCollisionSide = CollisionSide::LEFT;
                break;
            case CollisionSide::INVALID:
                reverseCollisionSide = CollisionSide::INVALID;
        }
        interactiveObject->interactWithSide(context, collidingObject, reverseCollisionSide, time);
    } else {
        if (tile != TileTypes::EMPTY) {
            interactiveObject->collideWithSide(context, tile, collisionWithStaticSide,
                                               time);
        }
    }
    return tile;
}

void World::stepSimulation(long time, std::shared_ptr<Context> context) {
    context->getPlayer()->step(context, time);
    int middleOfScreenPixel = SCREEN_WIDTH / 2;

    for (size_t i = 0; i < this->objects.size(); ++i) {
        if (context->getPlayer()->getPosition()->getMaxRight() + middleOfScreenPixel >
            this->objects[i]->getPosition()->getLeftBorder()) {
            this->objects[i]->step(context, time);
        }
    }

    for (size_t i = 0; i < this->objects.size(); ++i) {
        std::shared_ptr<InteractiveObject> interactiveObject = this->objects[i];
        stepSingleObject(time, context, interactiveObject);
    }

    for (unsigned int i = 0; i < objects.size(); i++) {
        if (objects[i]->waitingForDestroy()) {
            objects.erase(objects.begin() + i);
        }
    }
}

void World::load(std::string logicFile, int &error) {
    std::ifstream mapFile("./res/levels/" + logicFile);
    std::string line;
    if (mapFile.is_open()) {
        int lineNumber = 0;
        while (getline(mapFile, line)) {
            if (lineNumber > 14) {
                std::cerr << "The map height must be 15 or less, exiting.." << std::endl;
                exit(-1);
            }
            //std::cout << line << '\n';

            for (unsigned int i = 0; i < line.length(); i++) {
                tiles[i][lineNumber] = (TileTypes) (line.at(i) -
                                                    '0');//this removes char 0 to make ascii values match tile numbers
            }
            lineNumber++;
        }
        mapFile.close();
    } else {
        error = 1;
        return;
    }
    error = 0;
}

TileTypes World::getTileObject(int x, int y) {
    if (x < 0 || x >= 224 || y < 0 || y >= 15) {
        std::cerr << "the requested object is out of map range " << x << ", " << y << std::endl;
        return OUT_OF_MAP;
    }
    return tiles[x][y];
}

/**
 * if returns -1,-1, it means not found.
 * @param type
 * @return
 */
SDL_Rect World::getObject(TileTypes type) {
    SDL_Rect position;
    position.x = -1;
    position.y = -1;
    for (int i = 0; i < 224; i++) {
        for (int j = 0; j < 15; j++) {
            if (getTileObject(i, j) == type) {
                position.x = i;
                position.y = j;
                return position;
            }
        }
    }
    return position;
}

SDL_Rect World::getAndRemoveObject(TileTypes types) {
    SDL_Rect rect = getObject(types);
    if (rect.x != -1 && rect.y != -1) {
        tiles[rect.x][rect.y] = TileTypes::EMPTY;
    }

    return rect;
}