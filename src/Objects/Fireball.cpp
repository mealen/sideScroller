#include <SDL_render.h>
#include <SDL_mixer.h>

#include "../AABB.h"
#include "../Utils.h"
#include "InteractiveObject.h"
#include "Mario.h"
#include "CoinBox.h"
#include "Fireball.h"
#include "Brick.h"
#include "../Context.h"


Fireball::Fireball(SDL_Renderer *ren, int x, int y) {//FIXME this should not need  renderer and map
    collisionBox = new AABB(
            x,
            x + TILE_SIZE / 2,
            y,
            y + TILE_SIZE / 2); //-1 because 32 is not part of box. pixels 0 - TILE_SIZE, TILE_SIZE excluded

    collisionBox->setPhysicsState(AABB::PhysicsState::DYNAMIC);

    for (int i = 0; i < 4; i++) {
        std::string fireballImage = Utils::getResourcePath("fireball") + "fireball_" + std::to_string(i) + ".bmp";
        texture.push_back(Utils::loadTexture(ren, fireballImage));
    }


    //deadSound.push_back(Mix_LoadWAV("./res/sounds/stomp.wav"));
}

Fireball::~Fireball() {
    for (size_t i = 0; i < texture.size(); i++) {
        SDL_DestroyTexture(texture[i]);
    }
    delete(collisionBox);
}

SDL_Texture* Fireball::getTexture(long time __attribute((unused))) const {
    return texture[static_cast<int>(getDirection())];
}

AABB* Fireball::getPosition() const {
    return collisionBox;
}

TileTypes Fireball::getTileType() const {
    return TileTypes::FIREBALL;
}

void Fireball::render(SDL_Renderer* renderer, int x, int y, long time) {
    SDL_Rect screenPos;
    screenPos.x = collisionBox->getLeftBorder() - x;
    screenPos.y = collisionBox->getUpBorder() - y;
    screenPos.w = TILE_SIZE / 2;
    screenPos.h = TILE_SIZE / 2;

    SDL_RenderCopyEx(renderer, getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);

}

void Fireball::collideWithSide(std::shared_ptr<Context> context __attribute((unused)), TileTypes tile,
                               CollisionSide interactionSide, long time __attribute((unused))) {
    if (tile == TileTypes::GROUND) {
        firstTouch = true;

    } else if (interactionSide != CollisionSide::UP && interactionSide != CollisionSide::DOWN){
        die(getTileType());
    }

}

TileTypes Fireball::interactWithSide(std::shared_ptr<Context> context __attribute((unused)), std::shared_ptr<InteractiveObject> otherObject,
                                     CollisionSide interactionSide __attribute((unused)), long time __attribute((unused))) {
    if(hitTime != 0) {
        return TileTypes::FIREBALL;//if already interacted, don't allow again
    }

    if (otherObject->getTileType() != TileTypes::PLAYER && otherObject->getPosition()->getPhysicsState() != AABB::PhysicsState::STATIC) {
        die(getTileType());
    }


    return TileTypes::FIREBALL;//no interaction yet
}

bool Fireball::waitingForDestroy() {
    return isDead(); //there is no case we are expecting removal
}

void Fireball::changeDirection() {
    switch (direction) {
        case RIGHT_UP:
            direction = RIGHT_DOWN;
            break;
        case RIGHT_DOWN:
            direction = RIGHT_UP;
            break;
        case LEFT_UP:
            direction = LEFT_DOWN;
            break;
        case LEFT_DOWN:
            direction = LEFT_UP;
            break;
    }
};

void Fireball::step(std::shared_ptr<Context> context, long time __attribute((unused))) {
    int moveSpeed = 8;

    if (firstTouch) {
        moveSpeed = 16;
    }
    if(directionChangeRequested) {
        changeDirection();
        directionChangeRequested = false;
    }

    //if (directionRight) {

    switch (direction) {
        case RIGHT_UP:
        case RIGHT_DOWN:
            this->getPosition()->moveRight(moveSpeed);
            break;

        case LEFT_UP:
        case LEFT_DOWN:
            this->getPosition()->moveLeft(moveSpeed);
            break;
    }


    if (getPosition()->getRightBorder() > context->getEndOfScreen()) {
        die(TileTypes::EMPTY);
    }

}

void Fireball::setDirection(Fireball::Direction direction) {
    this->direction = direction;
}

Fireball::Direction Fireball::getDirection() const {
    return direction;
}