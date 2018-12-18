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
#include <SDL_mixer.h>
#include <SDL_rect.h>

#include "../Constants.h"
#include "../Utils.h"
#include "../Map.h"
#include "../AABB.h"
#include "InteractiveObject.h"

class Mario : public InteractiveObject {

public:
    enum Status {
        SMALL, BIG
    };
    enum TextureNames {
        STAND, MOVE, JUMP, DEAD
    };

private:
    std::map<Status, std::map<TextureNames, std::vector<SDL_Texture *>>> textures;
    TextureNames currentState = STAND;
    int screenWidth;
    int score = 0;
    int coins = 0;
    AABB* collisionBox;
    bool killed = false;
    bool isBig = false;
    bool growStarted = false;
    long growStartTime = 0;
    long lastStepTime = 0;
    bool moveRight = true;
    Mix_Chunk *growSound;
public:
    static const int MOVE_SPEED;
    static const int JUMP_SPEED;
    Mario(SDL_Rect mapPosition, SDL_Renderer *ren, int screenWidth, int &error);
    void render(SDL_Renderer *renderer, int x, int y, long time) override;

    ~Mario() {
        SDL_DestroyTexture(textures[SMALL][STAND][0]);
        SDL_DestroyTexture(textures[SMALL][MOVE][0]);
        SDL_DestroyTexture(textures[SMALL][MOVE][1]);
        SDL_DestroyTexture(textures[SMALL][MOVE][2]);
        SDL_DestroyTexture(textures[SMALL][JUMP][0]);
        SDL_DestroyTexture(textures[BIG][STAND][0]);
        SDL_DestroyTexture(textures[BIG][MOVE][0]);
        SDL_DestroyTexture(textures[BIG][MOVE][1]);
        SDL_DestroyTexture(textures[BIG][MOVE][2]);
        SDL_DestroyTexture(textures[BIG][JUMP][0]);
        delete collisionBox;
    }

    SDL_Texture *getTexture(long time) const {
        Status curStatus;
        if (isBig) {
            curStatus = BIG;
        } else {
            curStatus = SMALL;
        }
        if(collisionBox->isHasJumped()) {
            return textures.at(curStatus).at(JUMP).at(0);
        }
        if (isDead()) {
            return textures.at(curStatus).at(DEAD).at(0);
        }
        switch (currentState) {
            case STAND:
                return textures.at(curStatus).at(STAND).at(0);
            case MOVE:
                return textures.at(curStatus).at(MOVE).at((time / 75) % 3);
            default:
                std::cerr << "Requested Texture type not found" << std::endl;
                exit(-1);
        }
    }

    AABB* getPosition() const {
        return collisionBox;
    }

    void move(bool left, bool right, bool jump, bool crouch) {
        if (growStarted) {
            return;
        }
        if(isDead()) {
            return;
        }
        if(jump) {
            collisionBox->jump(JUMP_SPEED);
        }
        if (left) {
            moveRight = false;
            currentState = MOVE;
            if (collisionBox->getLeftBorder() + (320) > collisionBox->getMaxRight()) {
                collisionBox->moveLeft(MOVE_SPEED);
            }
        }
        if (right) {
            moveRight = true;
            currentState = MOVE;
            collisionBox->moveRight(MOVE_SPEED);
        }
        if(!left && !right) {
            currentState = STAND;
        }
    }

    Map::TileTypes getTileType() const {
        return Map::PLAYER;
    }

    Map::TileTypes interactWithSide(std::shared_ptr<Context> context, std::shared_ptr<InteractiveObject> otherObject,
                                    int interactionSide, long time) {
        if (otherObject->getTileType() == Map::TileTypes::GOOMBA && interactionSide != 1) {
            die(otherObject->getTileType());
        }
        if (otherObject->getTileType() == Map::TileTypes::MUSHROOM) {
            grow();
        }
        return this->getTileType();//no interaction yet
    };
    bool waitingForDestroy() {
        return false; //there is no case we are expecting removal
    };


    void step(long time) {
        if(!isDead()) {
            lastStepTime = time;
        }
        if (growStarted && growStartTime == 0) {
            Mix_PlayChannel(-1, growSound, 0);
            growStartTime = time;
        }

        if (growStarted) {
            getPosition()->setPhysicsState(AABB::STATIC);
            if (((time - growStartTime) / 100 ) % 2) {
                shrink();
            } else {
                grow();
            }
        } else if (!killed) {
            getPosition()->setPhysicsState(AABB::DYNAMIC);

        }

        if (time - growStartTime > 1000) {
            growStarted = false;
            growStartTime = 0;
        }
    };

    int getScore() const {
        return score;
    }

    int getCoins() const {
        return coins;
    }

    int increaseScore(int amount = 1) {
        score += amount;
        return score;
    }

    int increaseCoin(int amount = 1) {
        coins += amount;
        return coins;
    }

    void die(Map::TileTypes type) {
        if (getBig()) {
            shrink();
            return;
        }
        if (isDead()) {
            return;
        }
        InteractiveObject::die(type);
        if (type == Map::TileTypes::GOOMBA) {
            //getPosition()->setUpBorder(collisionBox->getUpBorder()+TILE_SIZE);
            getPosition()->setUpwardSpeed(Mario::JUMP_SPEED / 2);
            //collisionBox->jump(JUMP_SPEED);
            killed = true;
            getPosition()->setPhysicsState(AABB::KINEMATIC);
            getPosition()->setHorizontalSpeed(0);
        }
    }

    bool isKilled() const;

    long getDeadTime() const {
        return lastStepTime;

    }

    bool grow() {
        if (!isBig) {
            growStarted = true;
            isBig = true;
            getPosition()->setUpBorder(getPosition()->getUpBorder() - TILE_SIZE);
            return true;
        } else {
            return false;
        }
    }

    bool shrink() {
        if (isBig) {
            isBig = false;
            getPosition()->setUpBorder(getPosition()->getUpBorder() + TILE_SIZE);
            return true;
        } else {
            return false;
        }
    }

    bool getBig() {
        return isBig;
    }

    bool isGrowStarted() const;

};


#endif //MARIO_MARIO_H
