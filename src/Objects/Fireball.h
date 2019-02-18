#ifndef MARIO_FIREBALL_H
#define MARIO_FIREBALL_H


#include <SDL_render.h>
#include <SDL_mixer.h>

#include "../AABB.h"
#include "../Utils.h"
#include "InteractiveObject.h"
#include "../Constants.h"
#include "Mario.h"
#include "Coin.h"

class Fireball : public InteractiveObject {
public:
    enum Direction { LEFT_UP = 0, RIGHT_UP = 1, RIGHT_DOWN = 2, LEFT_DOWN = 3 };
private:
    std::vector<SDL_Texture *> texture;
    AABB* collisionBox;
    long hitTime = 0;
    Direction direction;
    std::vector<Mix_Chunk *>deadSound;
    bool directionChangeRequested = false;
    bool firstTouch = false;

public:

    Fireball(SDL_Renderer *ren, int x, int y);

    ~Fireball();

    SDL_Texture* getTexture(long time __attribute((unused))) const override;

    AABB* getPosition() const override;

    TileTypes getTileType() const override;

    void render(SDL_Renderer* renderer, int x, int y, long time) override;

    void collideWithSide(std::shared_ptr<Context> context __attribute((unused)), TileTypes tile,
                         CollisionSide interactionSide, long time __attribute((unused))) override;

    TileTypes interactWithSide(std::shared_ptr<Context> context __attribute((unused)), std::shared_ptr<InteractiveObject> otherObject,
                               CollisionSide interactionSide, long time __attribute((unused))) override;

    bool waitingForDestroy() override;

    void step(std::shared_ptr<Context> context __attribute((unused)), long time __attribute((unused))) override;

    void setCreateTime(long time);

    void setDirection(Direction direction);

    Direction getDirection() const;

    void changeDirection();


};


#endif //MARIO_FIREBALL_H
