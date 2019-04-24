//
// Created by engin on 11.11.2017.
//

#ifndef MARIO_BRICK_H
#define MARIO_BRICK_H

#include <SDL_render.h>
#include <SDL_mixer.h>

#include "../AABB.h"
#include "../Utils.h"
#include "BoxBase.h"

class Brick : public BoxBase {
    SDL_Texture *texture;
    AABB* collisionBox;
    long breakTime = 0;
    long hitTime = 0;
    Mix_Chunk *breakSound = NULL;
    Mix_Chunk *hitSound = NULL;
    bool breakFramePassed = false;
    bool isDestroyed = false;
    const int HIT_ANIMATION_TIME = 250;

public:
    Brick(SDL_Renderer *ren, int x, int y) {//FIXME this should not need  renderer and map
        collisionBox = new AABB(
                x * TILE_SIZE,
                x * TILE_SIZE + TILE_SIZE -1,
                y * TILE_SIZE,
                y * TILE_SIZE + TILE_SIZE -1); //-1 because 32 is not part of box. pixels 0 - TILE_SIZE, TILE_SIZE excluded


        std::string marioImage = Utils::getResourcePath("brick") + "brickred.bmp";
        texture = Utils::loadTexture(ren, marioImage);
        breakSound = Mix_LoadWAV("./res/sounds/blockbreak.wav");
        hitSound = Mix_LoadWAV("./res/sounds/blockhit.wav");
    }

    ~Brick() {
        SDL_DestroyTexture(texture);
        delete(collisionBox);

    }

    SDL_Texture* getTexture(long time  __attribute((unused))) const {
        return texture;
    };

    AABB* getPosition() const {
        return collisionBox;
    };

    TileTypes getTileType() const {
        if(this->breakFramePassed) {
            return TileTypes::EMPTY;// if breaking, don't interact
        }
        return TileTypes::BRICK;
    }

    void render(SDL_Renderer* renderer, int x, int y, long time) {
        SDL_Rect screenPos;
        screenPos.x = collisionBox->getLeftBorder() - x;
        screenPos.y = collisionBox->getUpBorder() - y;
        screenPos.w = TILE_SIZE;
        screenPos.h = TILE_SIZE;
        if(breakTime != 0 && breakTime != time) {

            SDL_Rect texturePos;
            screenPos.w = texturePos.w = TILE_SIZE/2;
            screenPos.h = texturePos.h = TILE_SIZE/2;
            texturePos.x = 0;
            texturePos.y = 0;
            long animTime = time - breakTime;
            if (animTime > 17) {
                breakFramePassed = true;
            }
            if(animTime > 500) {
                isDestroyed = true;
                animTime=500;//stop animation after 500
            }
            float upSpeed = sin(M_PI * (animTime)/500.0f);
            screenPos.y = screenPos.y - upSpeed*32;

            screenPos.x =screenPos.x - animTime / 15;
            //up left
            SDL_RenderCopyEx(renderer, getTexture(time), &texturePos, &screenPos, -animTime, 0, SDL_FLIP_NONE);
            screenPos.x += TILE_SIZE/2 + 2 *(animTime / 16);
            texturePos.x = TILE_SIZE/2;
            //up right
            SDL_RenderCopyEx(renderer, getTexture(time), &texturePos, &screenPos, animTime, 0, SDL_FLIP_NONE);
            texturePos.y = TILE_SIZE /2;
            screenPos.y += TILE_SIZE/2 + animTime /32;
            //down right
            SDL_RenderCopyEx(renderer, getTexture(time), &texturePos, &screenPos, animTime, 0, SDL_FLIP_NONE);
            texturePos.x = 0;
            screenPos.x -= TILE_SIZE/2 + 2 *(animTime / 16);
            //down left
            SDL_RenderCopyEx(renderer, getTexture(time), &texturePos, &screenPos, -animTime, 0, SDL_FLIP_NONE);
        } else if(hitTime != 0 && hitTime != time) {
            SDL_Rect texturePos;
            screenPos.w = texturePos.w = TILE_SIZE;
            screenPos.h = texturePos.h = TILE_SIZE;
            texturePos.x = 0;
            texturePos.y = 0;
            long animTime = time - hitTime;
            if(animTime > HIT_ANIMATION_TIME) {
                animTime = HIT_ANIMATION_TIME;//stop animation after 500
                whileHit = false;
            }
            float upSpeed = sin(M_PI * (animTime) / HIT_ANIMATION_TIME);
            screenPos.y = screenPos.y - upSpeed * (TILE_SIZE / 4);

            SDL_RenderCopyEx(renderer, getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);
        } else {
            SDL_RenderCopyEx(renderer, getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);
        }
    }

    TileTypes interactWithSide(std::shared_ptr<Context> context __attribute((unused)), std::shared_ptr<InteractiveObject> otherObject __attribute((unused)),
                                    CollisionSide interactionSide, long time) {
        if(breakTime != 0) {
            return this->getTileType();//if already interacted, don't allow again
        }
        if(interactionSide == CollisionSide::DOWN) {
            if (otherObject.get()->getTileType() == TileTypes::PLAYER && (static_cast<Mario *>(otherObject.get()))->isBig()) {
                //isDestroyed = true;
                Mix_PlayChannel(-1, breakSound, 0);
                breakTime = time;
            } else {
                Mix_PlayChannel(-1, hitSound, 0);
                hitTime = time;
            }
            whileHit = true;
        }
        return this->getTileType();//no interaction yet
    };

    bool waitingForDestroy()  const {
        return isDestroyed; //there is no case we are expecting removal
    };

    void step(std::shared_ptr<Context> context __attribute((unused)), long time __attribute((unused))) {};



};


#endif //MARIO_BRICK_H
