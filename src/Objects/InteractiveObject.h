//
// Created by engin on 11.11.2017.
//

#ifndef MARIO_DYNAMICOBJECT_H
#define MARIO_DYNAMICOBJECT_H


#include <SDL_render.h>
#include "../AABB.h"
#include "../Map.h"

class InteractiveObject {
public:
    virtual AABB* getPosition() const = 0;
    virtual SDL_Texture* getTexture(long time) const= 0;
    virtual Map::TileTypes getTileType() const = 0;
    virtual void render(SDL_Renderer* renderer, int x, int y, long time) = 0;

    virtual int interactWithSide(int interactionSide, long time, InteractiveObject *playerObject) = 0;
    virtual void step(long time) = 0;
    virtual bool waitingForDestroy() = 0;

    virtual ~InteractiveObject() {};


};


#endif //MARIO_DYNAMICOBJECT_H
