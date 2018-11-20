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

    virtual int interactWithSide(std::shared_ptr<Context> context, int interactionSide, long time) = 0;
    virtual void step(long time) = 0;
    virtual bool waitingForDestroy() = 0;

    virtual ~InteractiveObject() {};


};


#endif //MARIO_DYNAMICOBJECT_H
