//
// Created by engin on 29.10.2017.
//

#ifndef MARIO_MARIO_H
#define MARIO_MARIO_H

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

#include <SDL2/SDL.h>
#include <SDL_mixer.h>
#include <SDL_rect.h>

#include "../Constants.h"
#include "../Utils.h"
#include "../AABB.h"
#include "InteractiveObject.h"

class Mario : public InteractiveObject {

public:
    enum Status {
        SMALL, BIG
    };
    enum TextureNames {
        STAND, MOVE, JUMP, DEAD
    };

private:
    std::map<Status, std::map<TextureNames, std::vector<SDL_Texture *>>> textures;
    TextureNames currentState = STAND;
    int score = 0;
    int coins = 0;
    AABB* collisionBox;
    bool killed = false;
    bool isBig = false;
    bool growStarted = false;
    long growStartTime = 0;
    bool shrinkStarted = false;
    long shrinkStartTime = 0;
    long lastStepTime = 0;
    bool moveRight = true;
    Mix_Chunk *growSound;
public:
    static const int MOVE_SPEED;
    static const int JUMP_SPEED;
    Mario(SDL_Rect mapPosition, SDL_Renderer *ren, int &error);
    void render(SDL_Renderer *renderer, int x, int y, long time) override;

    ~Mario();

    SDL_Texture *getTexture(long time) const override;

    AABB* getPosition() const override;

    void move(bool left, bool right, bool jump, bool crouch __attribute((unused)));

    TileTypes getTileType() const override;

    TileTypes interactWithSide(std::shared_ptr<Context> context __attribute((unused)), std::shared_ptr<InteractiveObject> otherObject,
                                    int interactionSide, long time __attribute((unused))) override;

    bool waitingForDestroy() override;

    void step(long time) override;


    int getCoins() const;


    int increaseCoin(int amount = 1);

    void die(TileTypes type) override;

    bool isKilled() const;

    long getDeadTime() const;

    bool grow();

    bool shrink();

    bool getBig();

    bool isGrowStarted() const;

    bool isShrinkStarted() const;

};


#endif //MARIO_MARIO_H
