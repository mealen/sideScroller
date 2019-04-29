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
class Context;

class Mario : public InteractiveObject {

public:
    enum Status {
        SMALL, BIG
    };
    enum Color {
        NORMAL, BLACK, WHITE
    };
    enum TextureNames {
        STAND, MOVE, JUMP, DEAD, CROUCH, POLE
    };

private:
    std::map<Status, std::map<Color, std::map<TextureNames, std::vector<SDL_Texture *>>>> textures;
    TextureNames currentState = STAND;
    int score = 0;
    int coins = 0;
    int moveSpeed = 4;
    AABB *collisionBox;
    Status status = SMALL;
    Color color = NORMAL;
    bool killed = false;
    bool star = false;
    bool growStarted = false;
    bool fire = false;
    long growStartTime = 0;
    bool shrinkStarted = false;
    long shrinkStartTime = 0;
    long lastStepTime = 0;
    long fireStartTime = 0;
    long starStartTime = 0;
    long fireAnimationStartTime = 0;
    uint32_t mapWidth;
    bool moveRight = true;
    bool fireTriggered = false;
    int jumpSoundChannel = -1;
    Mix_Chunk *growSound = nullptr;
    Mix_Chunk *jumpSound = nullptr;
    Mix_Chunk *jumpBigSound = nullptr;
public:
    static const int JUMP_SPEED = 16;

    Mario(SDL_Rect mapPosition, SDL_Renderer *ren, uint32_t mapWidth, int &error);

    void render(SDL_Renderer *renderer, int x, int y, long time) override;

    ~Mario();

    SDL_Texture *getTexture(long time) const override;

    AABB *getPosition() const override;

    void setPosition(int xPosition, int yPosition);

    void move(bool left, bool right, bool jump, bool crouch __attribute((unused)), bool run);

    TileTypes getTileType() const override;

    TileTypes interactWithSide(std::shared_ptr<Context> context __attribute((unused)),
                               std::shared_ptr<InteractiveObject> otherObject,
                               CollisionSide interactionSide, long time __attribute((unused))) override;

    bool waitingForDestroy() const override;

    void step(std::shared_ptr<Context> context, long time) override;

    int getCoins() const;

    int getScore() const;

    int increaseCoin(int amount = 1);

    void die(TileTypes type) override;

    bool isKilled() const;

    long getDeadTime() const;

    bool grow();

    bool shrink();

    bool isBig() const;

    bool isGrowStarted() const;

    bool isShrinkStarted() const;

    bool isRunning() const;

    void setRunning(bool run);

    void setFire(bool fire, long time);

    bool canFire() const;

    void setStar(long starTime);

    bool getStar() const;

    bool isFireAnimationStarted() const;

    Status getStatus();

    std::string enumToName(Mario::Status status);

    std::string enumToName(Mario::Color status);

    std::string enumToName(Mario::TextureNames status);

    int increaseScore(int amount);

    void copyData(Mario& otherMario) {
        otherMario.score = score;
        otherMario.coins = coins;
        otherMario.status = status;
        if(isBig()) {
            otherMario.getPosition()->setUpBorder(otherMario.getPosition()->getUpBorder() - TILE_SIZE);
        }
    }

    void setState(TextureNames state) {
        this->currentState = state;
    }
};

#endif //MARIO_MARIO_H
