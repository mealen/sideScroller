//
// Created by engin on 11.11.2017.
//

#ifndef MARIO_GOOMBA_H
#define MARIO_GOOMBA_H


#include <SDL_render.h>
#include <SDL_mixer.h>

#include "../AABB.h"
#include "../Utils.h"
#include "InteractiveObject.h"
#include "Mario.h"
#include "Coin.h"

class Goomba : public InteractiveObject {
    std::vector<SDL_Texture *> texture;
    AABB* collisionBox;
    long hitTime = 0;
    std::vector<Mix_Chunk *>deadSound;
    bool isSquashed = false;
    bool isRemoveWaiting = false;
    bool directionRight = false;
    bool directionChangeRequested = false;

public:
    Goomba(SDL_Renderer *ren, int x, int y) {//FIXME this should not need  renderer and map
        collisionBox = new AABB(
                x * TILE_SIZE,
                x * TILE_SIZE + TILE_SIZE -1,
                y * TILE_SIZE,
                y * TILE_SIZE + TILE_SIZE -1); //-1 because 32 is not part of box. pixels 0 - TILE_SIZE, TILE_SIZE excluded

        collisionBox->setPhysicsState(AABB::PhysicsState::DYNAMIC);
        for (int i = 0; i < 2; i++) {
            std::string goombaImage = Utils::getResourcePath("goomba") + "goomba_" + std::to_string(i) + ".bmp";
            texture.push_back(Utils::loadTexture(ren, goombaImage));
        }

        std::string goombaImage = Utils::getResourcePath("goomba") + "goomba_dead.bmp";
        texture.push_back(Utils::loadTexture(ren, goombaImage));

        deadSound.push_back(Mix_LoadWAV("./res/sounds/stomp.wav"));
    }

    ~Goomba() {
        for (size_t i = 0; i < texture.size(); i++) {
            SDL_DestroyTexture(texture[i]);
        }
        delete(collisionBox);
    }

    SDL_Texture* getTexture(long time) const {
        if (this->isSquashed) {
            return texture[2];
        }

        return texture[(time / 250) % 2];
    };

    AABB* getPosition() const {
        return collisionBox;
    };

    Map::TileTypes getTileType() const {
        return Map::GOOMBA;
    }

    void render(SDL_Renderer* renderer, int x, int y, long time) {
        SDL_Rect screenPos;
        screenPos.x = collisionBox->getLeftBorder() - x;
        screenPos.y = collisionBox->getUpBorder() - y;
        screenPos.w = TILE_SIZE;
        screenPos.h = TILE_SIZE;

        SDL_RenderCopyEx(renderer, getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);

    }

    void collideWithSide(std::shared_ptr<Context> context, Map::TileTypes tile,
                                    int interactionSide, long time) override {
        if (interactionSide == 1) {

        }
        else if (interactionSide == 3 || interactionSide == 4 || interactionSide == -1) {
            if(tile != Map::GROUND) {
                directionRight = !directionRight;
            }
        }

    }

    Map::TileTypes interactWithSide(std::shared_ptr<Context> context, std::shared_ptr<InteractiveObject> otherObject,
                                    int interactionSide, long time) {
        if(hitTime != 0) {
            return Map::GOOMBA;//if already interacted, don't allow again
        }

        // if mario is coming from top, kill
        if(interactionSide == 2 && otherObject->getTileType() == Map::TileTypes::PLAYER) {
            if(!otherObject->isDead()) {
                isSquashed = true;
                collisionBox->setDownBorder(collisionBox->getDownBorder() - TILE_SIZE / 2);
                collisionBox->setPhysicsState(AABB::PhysicsState::STATIC);
                otherObject->getPosition()->setUpwardSpeed(Mario::JUMP_SPEED / 2);

                for (uint32_t i = 0; i < deadSound.size(); ++i) {
                    Mix_PlayChannel(-1, deadSound[i], 0);
                }

                hitTime = time;
                die(getTileType());
                return Map::EMPTY;
            }
            // swap direction
        } else if (interactionSide == 3 || interactionSide == 4) {
            if (otherObject->getTileType() == Map::TileTypes::PLAYER) {
                otherObject->die(getTileType());
            } else {
                if(!directionChangeRequested) {
                    directionRight = !directionRight;
                    if (otherObject->getTileType() == Map::TileTypes::GOOMBA) {
                        Goomba *otherGoomba = static_cast<Goomba *>(otherObject.get());
                        otherGoomba->directionChangeRequested = true;
                    }
                }

            }
        }


        return Map::GOOMBA;//no interaction yet
    }

    bool waitingForDestroy() {
        return isRemoveWaiting; //there is no case we are expecting removal
    };

    void step(long time) {
        if(directionChangeRequested) {
            directionRight = !directionRight;
            directionChangeRequested = false;
        }
        if(!isSquashed) {
            if (directionRight) {
                this->getPosition()->moveRight(1);
            } else {
                this->getPosition()->moveLeft(1);
            }
        }
        if(hitTime != 0 && time - hitTime >= 250) {
            isRemoveWaiting = true;
        }
    };

};


#endif //MARIO_GOOMBA_H
