//
// Created by mesutcan on 11/20/18.
//

#ifndef MARIO_COIN_H
#define MARIO_COIN_H


#include "InteractiveObject.h"

class HiddenCoin: public InteractiveObject {
    std::vector<SDL_Texture*> texture;
    AABB* collisionBox;
    long firstRenderTime = 0;
    bool isDestroyed = false;

public:
    HiddenCoin(SDL_Renderer *ren, int x, int y);

    AABB *getPosition() const override;

    SDL_Texture *getTexture(long time) const override;

    Map::TileTypes getTileType() const override;

    void render(SDL_Renderer *renderer, int x, int y, long time) override;

    int interactWithSide(std::shared_ptr<Context> context, int interactionSide, long time) override;

    void step(long time) override;

    bool waitingForDestroy() override;
};


#endif //MARIO_COIN_H
