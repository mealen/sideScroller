//
// Created by mesutcan on 11/20/18.
//

#ifndef MARIO_COIN_H
#define MARIO_COIN_H

#include <vector>

#include "InteractiveObject.h"

class Coin: public InteractiveObject {
    std::vector<SDL_Texture*> texture;
    AABB* collisionBox;
    long firstRenderTime = 0;
    bool isDestroyed = false;
    bool isStatic = true;
    const long TOTAL_ANIM_TIME = 600l;
    const float SHRINK_FACTOR = 0.75f;

public:
    Coin(SDL_Renderer *ren, int x, int y, bool isStatic);

    AABB *getPosition() const override;

    SDL_Texture *getTexture(long time) const override;

    TileTypes getTileType() const override;

    void render(SDL_Renderer *renderer, int x, int y, long time) override;

    TileTypes interactWithSide(std::shared_ptr<Context> context, std::shared_ptr<InteractiveObject> otherObject,
                               CollisionSide interactionSide, long time) override;

    void step(std::shared_ptr<Context> context __attribute((unused)), long time) override;

    bool waitingForDestroy() override;

    void die();
};


#endif //MARIO_COIN_H
