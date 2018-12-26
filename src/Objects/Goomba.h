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
    Goomba(SDL_Renderer *ren, int x, int y);

    ~Goomba();

    SDL_Texture* getTexture(long time) const override;

    AABB* getPosition() const override;

    Map::TileTypes getTileType() const override;

    void render(SDL_Renderer* renderer, int x, int y, long time) override;

    void collideWithSide(std::shared_ptr<Context> context, Map::TileTypes tile,
                                    int interactionSide, long time) override;

    Map::TileTypes interactWithSide(std::shared_ptr<Context> context, std::shared_ptr<InteractiveObject> otherObject,
                                    int interactionSide, long time) override;

    bool waitingForDestroy() override;

    void step(long time) override;

};


#endif //MARIO_GOOMBA_H
