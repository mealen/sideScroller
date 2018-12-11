//
// Created by engin on 11.11.2017.
//

#ifndef MARIO_BRICK_MUSHROOM_H
#define MARIO_BRICK_MUSHROOM_H


#include <SDL_render.h>
#include <SDL_mixer.h>

#include "../AABB.h"
#include "../Utils.h"
#include "InteractiveObject.h"
#include "Mario.h"
#include "HiddenCoin.h"

class World;
class Mushroom;

class BrickMushroom : public InteractiveObject {
    std::vector<SDL_Texture *> texture;
    AABB* collisionBox;
    long hitTime = 0;
    std::vector<Mix_Chunk *>breakSound;
    bool isUsed = false;
    bool mushroomCreated = false;
    std::shared_ptr<World> worldTemp = nullptr;
    std::shared_ptr<Mushroom> mushroomTemp = nullptr;

public:
    BrickMushroom(SDL_Renderer *ren, int x, int y) {//FIXME this should not need  renderer and map
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

        breakSound.push_back(Mix_LoadWAV("./res/sounds/blockhit.wav"));
        breakSound.push_back(Mix_LoadWAV("./res/sounds/mushroomappear.wav"));
    }

    ~BrickMushroom() {
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

    Map::TileTypes getTileType() const {
        if (this->isUsed) {
            return Map::COIN_TAKEN;
        }
        return Map::BRICK_MUSHROOM;
    }

    void render(SDL_Renderer* renderer, int x, int y, long time);

    Map::TileTypes interactWithSide(std::shared_ptr<Context> context, std::shared_ptr<InteractiveObject> otherObject,
                                    int interactionSide, long time);

    bool waitingForDestroy() {
        return false; //there is no case we are expecting removal
    };

    void step(long time) {};

};


#endif //MARIO_BRICK_MUSHROOM_H
