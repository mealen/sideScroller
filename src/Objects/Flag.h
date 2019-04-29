//
// Created by engin on 24.04.2019.
//

#ifndef MARIO_FLAG_H
#define MARIO_FLAG_H


#include "InteractiveObject.h"

class Flag : public InteractiveObject {
    AABB* collisionBox;
    SDL_Texture * texture;
    bool activated = false;
public:

    Flag(SDL_Renderer *ren, int x, int y);

    ~Flag();

    AABB *getPosition() const override {
        return collisionBox;
    }

    SDL_Texture *getTexture(long time) const override {
        return texture;
    }

    TileTypes getTileType() const override {
        return TileTypes::FLAG;
    }

    void render(SDL_Renderer *renderer, int x, int y, long time) override;

    TileTypes interactWithSide(std::shared_ptr<Context> context, std::shared_ptr<InteractiveObject> otherObject,
                               CollisionSide interactionSide, long time) override;

    void step(std::shared_ptr<Context> context, long time) override;

    bool waitingForDestroy() const override {
        return false;
    }
};


#endif //MARIO_FLAG_H
