//
// Created by engin on 26.12.2018.
//

#ifndef MARIO_HIDDENCOINBOX_H
#define MARIO_HIDDENCOINBOX_H


#include <SDL_mixer.h>
#include <vector>
#include "InteractiveObject.h"
#include "../Utils.h"

/**
 * Coin box that looks like normal brick, gives coins for n seconds
 */
class HiddenCoinBox : public InteractiveObject {
    SDL_Texture * texture;
    AABB* collisionBox;
    long firstHitTime = 0;
    long lastHitTime = 0;
    long breakTime = 0;
    const long activeFor = 5000;//after first hit, active for this many seconds
    std::vector<Mix_Chunk *> breakSound;
    bool isUsed = false;
    bool isBroken = false;

public:
    HiddenCoinBox(SDL_Renderer *ren, int x, int y) {//FIXME this should not need  renderer and map
        collisionBox = new AABB(
                x * TILE_SIZE,
                x * TILE_SIZE + TILE_SIZE -1,
                y * TILE_SIZE,
                y * TILE_SIZE + TILE_SIZE -1); //-1 because 32 is not part of box. pixels 0 - TILE_SIZE, TILE_SIZE excluded


            std::string brickImage = Utils::getResourcePath("brick") + "brickred.bmp";
            texture =Utils::loadTexture(ren, brickImage);

        breakSound.push_back(Mix_LoadWAV("./res/sounds/coin.wav"));
        breakSound.push_back(Mix_LoadWAV("./res/sounds/blockhit.wav"));
    }

    ~HiddenCoinBox() {
        SDL_DestroyTexture(texture);
        delete(collisionBox);

    }

    SDL_Texture* getTexture(long time __attribute((unused))) const {
        return texture;
    };

    AABB* getPosition() const {
        return collisionBox;
    };

    TileTypes getTileType() const {
        if (this->isUsed) {
            return TileTypes::BRICK;
        }
        return TileTypes::HIDDEN_COIN_BOX;
    }

    void render(SDL_Renderer* renderer, int x, int y, long time) {
        if(breakTime != 0 && breakTime != time) {
            SDL_Rect screenPos;
            SDL_Rect texturePos;
            screenPos.w = texturePos.w = TILE_SIZE/2;
            screenPos.h = texturePos.h = TILE_SIZE/2;
            screenPos.x = collisionBox->getLeftBorder() - x;
            screenPos.y = collisionBox->getUpBorder() - y;
            texturePos.x = 0;
            texturePos.y = 0;
            long animTime = time - breakTime;
            if(animTime > 500) {
                isBroken = true;
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
        } else {
            SDL_Rect screenPos;
            screenPos.x = collisionBox->getLeftBorder() - x;
            screenPos.y = collisionBox->getUpBorder() - y;
            screenPos.w = TILE_SIZE;
            screenPos.h = TILE_SIZE;
            if (lastHitTime != 0 && lastHitTime != time) {
                long animTime = time - lastHitTime;
                if (animTime > 200) {
                    animTime = 200;//stop animation after 500
                }
                float upSpeed = static_cast<float>(sin(M_PI * (animTime) / 200.0f));
                screenPos.y = static_cast<int>(screenPos.y - upSpeed * 8);

                //coinImgPos.x = coinImgPos.x - animTime / 15;
                SDL_RenderCopyEx(renderer, getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);

            } else {
                SDL_RenderCopyEx(renderer, getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);
            }
        }
    }

    TileTypes interactWithSide(std::shared_ptr<Context> context, std::shared_ptr<InteractiveObject> otherObject,
                               CollisionSide interactionSide, long time) override;

    bool waitingForDestroy() override {
        return isBroken;
    };

    void step(long time );

};

#endif //MARIO_HIDDENCOINBOX_H
