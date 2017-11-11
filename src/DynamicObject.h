//
// Created by engin on 11.11.2017.
//

#ifndef MARIO_DYNAMICOBJECT_H
#define MARIO_DYNAMICOBJECT_H


#include <SDL_render.h>
#include "AABB.h"

class DynamicObject {
public:
    virtual const AABB* getPosition() const = 0;
    virtual SDL_Texture* getTexture(long time) const= 0;
};


#endif //MARIO_DYNAMICOBJECT_H
