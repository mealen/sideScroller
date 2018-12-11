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

#include "../Constants.h"
#include "../Utils.h"
#include "../Map.h"
#include "../AABB.h"
#include "InteractiveObject.h"

class Mario : public InteractiveObject {

public:
    enum TextureNames {
        STAND, MOVE, JUMP, DEAD
    };

private:
    std::map<TextureNames, std::vector<SDL_Texture *>> textures;
    TextureNames currentState = STAND;
    int screenWidth;
    int score = 0;
    int coins = 0;
    AABB* collisionBox;
    bool killed = false;
    long lastStepTime = 0;
public:
    static const int MOVE_SPEED;
    static const int JUMP_SPEED;
    Mario(SDL_Rect mapPosition, SDL_Renderer *ren, int screenWidth, int &error);

    ~Mario() {
        SDL_DestroyTexture(textures[STAND][0]);
        SDL_DestroyTexture(textures[MOVE][0]);
        SDL_DestroyTexture(textures[MOVE][1]);
        SDL_DestroyTexture(textures[MOVE][2]);
        SDL_DestroyTexture(textures[JUMP][0]);
        delete collisionBox;
    }

    SDL_Texture *getTexture(long time) const {
        if(collisionBox->isHasJumped()) {
            return textures.at(JUMP).at(0);
        }
        if (isDead()) {
            return textures.at(DEAD).at(0);
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

    AABB* getPosition() const {
        return collisionBox;
    }

    void move(bool left, bool right, bool jump, bool crouch) {
        if(isDead()) {
            return;
        }
        if(jump) {
            collisionBox->jump(JUMP_SPEED);
        }
        if (left) {
            currentState = MOVE;
            if (collisionBox->getLeftBorder() + (320) > collisionBox->getMaxRight()) {
                collisionBox->moveLeft(MOVE_SPEED);
            }
        }
        if (right) {
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
        return this->getTileType();//no interaction yet
    };
    bool waitingForDestroy() {
        return false; //there is no case we are expecting removal
    };

    void render(SDL_Renderer* renderer, int x, int y, long time) {
        //FIXME this is not used for rendering, but it should have
    };

    void step(long time) {
        if(!isDead()) {
            lastStepTime = time;
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
        if (isDead()) {
            return;
        }
        InteractiveObject::die(type);
        if (type == Map::TileTypes::GOOMBA) {
            //getPosition()->setUpBorder(collisionBox->getUpBorder()+TILE_SIZE);
            getPosition()->setUpwardSpeed(Mario::JUMP_SPEED);
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

};


#endif //MARIO_MARIO_H
