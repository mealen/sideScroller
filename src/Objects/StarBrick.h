#ifndef MARIO_STARBRICK_H
#define MARIO_STARBRICK_H



#include <SDL_render.h>
#include <SDL_mixer.h>

#include "../AABB.h"
#include "../Utils.h"
#include "BoxBase.h"
#include "Star.h"

class StarBrick : public BoxBase {
    std::vector<SDL_Texture *> texture;
    AABB* collisionBox;
    long breakTime = 0;
    long hitTime = 0;
    Mix_Chunk *breakSound = NULL;
    Mix_Chunk *hitSound = NULL;
    bool isUsed = false;
    const int HIT_ANIMATION_TIME = 250;
    std::shared_ptr<World> worldTemp = nullptr;
    std::shared_ptr<InteractiveObject> starTemp = nullptr;
    bool starCreated = false;


public:
    StarBrick(SDL_Renderer *ren, int x, int y) {//FIXME this should not need  renderer and map
        collisionBox = new AABB(
                x * TILE_SIZE,
                x * TILE_SIZE + TILE_SIZE -1,
                y * TILE_SIZE,
                y * TILE_SIZE + TILE_SIZE -1); //-1 because 32 is not part of box. pixels 0 - TILE_SIZE, TILE_SIZE excluded


        std::string brickImage = Utils::getResourcePath("brick") + "brickred.bmp";
        std::string usedBrickImage = Utils::getResourcePath("brick_coin") + "brick_coin_used.bmp";
        texture.push_back(Utils::loadTexture(ren, brickImage));
        texture.push_back(Utils::loadTexture(ren, usedBrickImage));
        breakSound = Mix_LoadWAV("./res/sounds/blockbreak.wav");
        hitSound = Mix_LoadWAV("./res/sounds/blockhit.wav");
    }

    ~StarBrick() {
        for (size_t i = 0; i < texture.size(); i++) {
            SDL_DestroyTexture(texture[i]);
        }
        delete(collisionBox);

    }

    SDL_Texture* getTexture(long time  __attribute((unused))) const {
        if (isUsed) {
            return texture[1];
        }
        else {
            return texture[0];
        }
    };

    AABB* getPosition() const {
        return collisionBox;
    };

    TileTypes getTileType() const {
        if (this->isUsed) {
            return TileTypes::COIN_TAKEN;
        }
        return TileTypes::STAR_BRICK;
    }

    void render(SDL_Renderer* renderer, int x, int y, long time) {
        SDL_Rect screenPos;
        screenPos.x = collisionBox->getLeftBorder() - x;
        screenPos.y = collisionBox->getUpBorder() - y;
        screenPos.w = TILE_SIZE;
        screenPos.h = TILE_SIZE;
        if(hitTime != 0 && hitTime != time) {
            long animTime = time - hitTime;
            long starTime = animTime;

            if(animTime > 200) {
                whileHit = false;
                animTime = 200;//stop animation after 500
            }
            float upSpeed = sin(M_PI * (animTime) / 200.0f);
            screenPos.y = collisionBox->getUpBorder() - y;//start from objects position
            screenPos.y = screenPos.y - upSpeed * 8;

            if(!starCreated) {
                if (starTime > 500) {
                    worldTemp->addObject(starTemp);
                    worldTemp = nullptr;
                    starTemp = nullptr;
                    starCreated = true;
                } else {
                    float upSpeed = 32 * starTime / 500;
                    SDL_Rect starPos = screenPos;
                    if(starPos.y > screenPos.y - upSpeed) {
                        starPos.y = screenPos.y - upSpeed;
                    }
                    SDL_RenderCopyEx(renderer, starTemp->getTexture(time), 0, &starPos, 0, 0, SDL_FLIP_NONE);
                }
            }


            SDL_RenderCopyEx(renderer, getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);

        } else {
            SDL_RenderCopyEx(renderer, getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);
        }
    }

    TileTypes interactWithSide(std::shared_ptr<Context> context __attribute((unused)), std::shared_ptr<InteractiveObject> otherObject __attribute((unused)),
                                    CollisionSide interactionSide, long time) {
        if(hitTime != 0) {
            return this->getTileType();//if already interacted, don't allow again
        }
        if(interactionSide == CollisionSide::DOWN) {
            //isDestroyed = true;
            Mix_PlayChannel(-1, breakSound, 0);

            if (!isUsed) {
                hitTime = time;
                whileHit = true;
                isUsed = true;
                std::shared_ptr<Mario> mario = context->getPlayer();
                if (mario != nullptr) {
                    worldTemp = context->getWorld();
                    starTemp = std::make_shared<Star>(context->getWorld()->getRen(),
                                                            this->getPosition()->getLeftBorder() / TILE_SIZE,
                                                            (this->getPosition()->getUpBorder() / TILE_SIZE) - 1);

                }
            }
        }
        return this->getTileType();
    }

    bool waitingForDestroy() {
        return false; //there is no case we are expecting removal
    }

    void step(std::shared_ptr<Context> context __attribute((unused)), long time __attribute((unused))) {};



};


#endif //MARIO_STARBRICK_H
