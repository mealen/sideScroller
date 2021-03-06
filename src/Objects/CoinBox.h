//
// Created by engin on 11.11.2017.
//

#ifndef MARIO_BRICK_COIN_H
#define MARIO_BRICK_COIN_H


#include <SDL_render.h>
#include <SDL_mixer.h>

#include "../AABB.h"
#include "../Utils.h"
#include "../Constants.h"
#include "Mario.h"
#include "Coin.h"
#include "BoxBase.h"

class CoinBox : public BoxBase {
    std::vector<SDL_Texture *> texture;
    AABB* collisionBox;
    long hitTime = 0;
    std::vector<Mix_Chunk *>breakSound;
    bool isUsed = false;

public:
    CoinBox(SDL_Renderer *ren, int x, int y) {//FIXME this should not need  renderer and map
        collisionBox = new AABB(
                x * TILE_SIZE,
                x * TILE_SIZE + TILE_SIZE -1,
                y * TILE_SIZE,
                y * TILE_SIZE + TILE_SIZE -1); //-1 because 32 is not part of box. pixels 0 - TILE_SIZE, TILE_SIZE excluded


        for (int i = 0; i < 3; i++) {
            std::string brickImage = Utils::getResourcePath("brick_coin") + "brick_coin" + std::to_string(i) + ".bmp";
            texture.push_back(Utils::loadTexture(ren, brickImage));
        }

        std::string brickImage = Utils::getResourcePath("brick_coin") + "brick_coin_used.bmp";
        texture.push_back(Utils::loadTexture(ren, brickImage));

        Mix_Chunk *sample;
        sample = Mix_LoadWAV("./res/sounds/coin.wav");
        if(!sample) {
            printf("Mix_LoadWAV: %s\n", Mix_GetError());
            // handle error
        } else {
            breakSound.push_back(sample);
        }
        sample = Mix_LoadWAV("./res/sounds/blockhit.wav");
        if(!sample) {
            printf("Mix_LoadWAV: %s\n", Mix_GetError());
            // handle error
        } else {
            breakSound.push_back(sample);
        }
    }

    ~CoinBox() {
        for (size_t i = 0; i < texture.size(); i++) {
            SDL_DestroyTexture(texture[i]);
        }

        delete(collisionBox);

    }

    SDL_Texture* getTexture(long time) const {
        if (this->isUsed) {
            return texture[3];
        }

        return texture[(time / 200) % 3];
    };

    AABB* getPosition() const {
        return collisionBox;
    };

    TileTypes getTileType() const {
        if (this->isUsed) {
            return TileTypes::COIN_TAKEN;
        }
        return TileTypes::COIN_BOX;
    }

    void render(SDL_Renderer* renderer, int x, int y, long time) {
        SDL_Rect screenPos;
        screenPos.x = collisionBox->getLeftBorder() - x;
        screenPos.y = collisionBox->getUpBorder() - y;
        screenPos.w = TILE_SIZE;
        screenPos.h = TILE_SIZE;
        if(hitTime != 0 && hitTime != time) {


            long animTime = time - hitTime;
            if(animTime > 200) {
                isUsed = true;
                this->whileHit = false;
                animTime = 200;//stop animation after 500
            }
            float upSpeed = sin(M_PI * (animTime) / 200.0f);
            screenPos.y = screenPos.y - upSpeed * 8;

            //coinImgPos.x = coinImgPos.x - animTime / 15;
            SDL_RenderCopyEx(renderer, getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);

        } else {
            SDL_RenderCopyEx(renderer, getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);
        }
    }

    TileTypes interactWithSide(std::shared_ptr<Context> context, std::shared_ptr<InteractiveObject> otherObject,
                               CollisionSide interactionSide, long time);

    bool waitingForDestroy() const {
        return false; //there is no case we are expecting removal
    };

    void step(std::shared_ptr<Context> context __attribute((unused)), long time __attribute((unused))) {};

};


#endif //MARIO_BRICK_COIN_H
