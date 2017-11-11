//
// Created by engin on 11.11.2017.
//

#ifndef MARIO_WORLD_H
#define MARIO_WORLD_H


#include "DynamicObject.h"

class World {
    std::vector<DynamicObject*> objects;

public:
    void addObject(DynamicObject* object) {
        objects.push_back(object);
    }

    /**
     *
     * @param ren  renderer
     * @param x -> current world x
     * @param y -> current world y
     * @param time -> get ticks
     */
    void render(SDL_Renderer *ren, int x, int y, long time) {
        for (unsigned int i = 0; i < objects.size(); ++i) {
            const AABB* pos = objects[i]->getPosition();
            SDL_Rect screenPos;
            screenPos.x = pos->getLeftBorder() - x;
            screenPos.y = pos->getUpBorder() - y;
            screenPos.w = TILE_SIZE;
            screenPos.h = TILE_SIZE;
            SDL_RenderCopyEx(ren, objects[i]->getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);
        }
    }
};


#endif //MARIO_WORLD_H


