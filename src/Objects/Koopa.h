//
// Created by engin on 5.02.2019.
//

#ifndef MARIO_KOOPA_H
#define MARIO_KOOPA_H

#include <SDL_render.h>
#include <vector>
#include <SDL_mixer.h>
#include "InteractiveObject.h"

class Koopa : public InteractiveObject {
    std::vector<SDL_Texture *> texture;
    AABB* collisionBox;
    long hideInShellTime = 0;
    long killHitTime = 0;
    std::vector<Mix_Chunk *>deadSound;
    bool isHideInShell = false;
    bool isShellMoving = false;
    bool isRemoveWaiting = false;
    bool directionRight = false;
    bool directionChangeRequested = false;
public:
    Koopa(SDL_Renderer *ren, int x, int y);

    ~Koopa();

    SDL_Texture* getTexture(long time) const override;

    AABB* getPosition() const override;

    TileTypes getTileType() const override;

    void render(SDL_Renderer* renderer, int x, int y, long time) override;

    void collideWithSide(std::shared_ptr<Context> context, TileTypes tile,
                         CollisionSide interactionSide, long time) override;

    TileTypes interactWithSide(std::shared_ptr<Context> context, std::shared_ptr<InteractiveObject> otherObject,
                               CollisionSide interactionSide, long time) override;

    bool waitingForDestroy() override;

    void step(std::shared_ptr<Context> context __attribute((unused)), long time) override;

    void die(TileTypes type) override;

    void moveShell(const CollisionSide &interactionSide);

    bool getShellMoving() const;
};


#endif //MARIO_KOOPA_H
