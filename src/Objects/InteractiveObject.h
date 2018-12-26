//
// Created by engin on 11.11.2017.
//

#ifndef MARIO_DYNAMICOBJECT_H
#define MARIO_DYNAMICOBJECT_H


#include <SDL_render.h>
#include "../AABB.h"
#include "../Map.h"
#include <memory>
class Context;

class InteractiveObject {
public:
    virtual AABB* getPosition() const = 0;
    virtual SDL_Texture* getTexture(long time) const= 0;
    virtual Map::TileTypes getTileType() const = 0;
    virtual void render(SDL_Renderer* renderer, int x, int y, long time) = 0;

    virtual Map::TileTypes interactWithSide(std::shared_ptr<Context> context,
                                            std::shared_ptr<InteractiveObject> otherObject, int interactionSide,
                                            long time) = 0;
    virtual void collideWithSide(std::shared_ptr<Context> context __attribute((unused)),
                                            Map::TileTypes tile __attribute((unused)), int interactionSide __attribute((unused)),
                                            long time __attribute((unused))) {};
    virtual void step(long time) = 0;
    virtual bool waitingForDestroy() = 0;

    virtual ~InteractiveObject() {};

    bool isDead() const {
        return dead;
    }

    virtual void die(Map::TileTypes tile __attribute((unused))) {
        this->dead = true;
    }

private:
    bool dead = false;

};


#endif //MARIO_DYNAMICOBJECT_H
