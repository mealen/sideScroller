//
// Created by engin on 11.11.2017.
//

#include <SDL_render.h>
#include <SDL_mixer.h>

#include "../AABB.h"
#include "../Utils.h"
#include "InteractiveObject.h"
#include "Mario.h"
#include "CoinBox.h"
#include "Mushroom.h"
#include "Brick.h"


Mushroom::Mushroom(SDL_Renderer *ren, int x, int y) {//FIXME this should not need  renderer and map
    collisionBox = new AABB(
            x * TILE_SIZE,
            x * TILE_SIZE + TILE_SIZE -1,
            y * TILE_SIZE,
            y * TILE_SIZE + TILE_SIZE -1); //-1 because 32 is not part of box. pixels 0 - TILE_SIZE, TILE_SIZE excluded

    collisionBox->setPhysicsState(AABB::PhysicsState::DYNAMIC);

    std::string goombaImage = Utils::getResourcePath("powerUps") + "mushroom.bmp";
    texture.push_back(Utils::loadTexture(ren, goombaImage));

    //deadSound.push_back(Mix_LoadWAV("./res/sounds/stomp.wav"));
}

Mushroom::~Mushroom() {
    for (size_t i = 0; i < texture.size(); i++) {
        SDL_DestroyTexture(texture[i]);
    }
    delete(collisionBox);
}

SDL_Texture* Mushroom::getTexture(long time __attribute((unused))) const {
    return texture[0];
}

AABB* Mushroom::getPosition() const {
    return collisionBox;
}

TileTypes Mushroom::getTileType() const {
    return TileTypes::MUSHROOM;
}

void Mushroom::render(SDL_Renderer* renderer, int x, int y, long time) {
    SDL_Rect screenPos;
    screenPos.x = collisionBox->getLeftBorder() - x;
    screenPos.y = collisionBox->getUpBorder() - y;
    screenPos.w = TILE_SIZE;
    screenPos.h = TILE_SIZE;

    SDL_RenderCopyEx(renderer, getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);

}

void Mushroom::collideWithSide(std::shared_ptr<Context> context __attribute((unused)), TileTypes tile,
                               CollisionSide interactionSide, long time __attribute((unused))) {
    if (interactionSide == CollisionSide::DOWN) {

    }
    else if (interactionSide == CollisionSide::LEFT || interactionSide == CollisionSide::RIGHT || interactionSide == CollisionSide::INVALID) {
        if(tile != TileTypes::GROUND) {
            directionRight = !directionRight;
        }
    }

}

TileTypes Mushroom::interactWithSide(std::shared_ptr<Context> context __attribute((unused)), std::shared_ptr<InteractiveObject> otherObject,
                                     CollisionSide interactionSide, long time __attribute((unused))) {
    if(hitTime != 0) {
        return TileTypes::MUSHROOM;//if already interacted, don't allow again
    }

    // if mario is coming from top, kill
    if(otherObject->getTileType() == TileTypes::PLAYER) {
        if(!otherObject->isDead()) {

            die(getTileType());
            Mario *player = static_cast<Mario *>(otherObject.get());
            player->grow();
            return TileTypes::EMPTY;
        }
        // swap direction
    } else if (interactionSide == CollisionSide::RIGHT || interactionSide == CollisionSide::LEFT) {
        if(!directionChangeRequested) {
            directionRight = !directionRight;
        }
    }

    if (otherObject->getTileType() == TileTypes::BRICK && interactionSide == CollisionSide::DOWN) {
        if ((static_cast<Brick *>(otherObject.get())->isWhileHit())) {
            this->getPosition()->jump(8);
        }
    }

    if (otherObject->getTileType() == TileTypes::COIN_BOX && interactionSide == CollisionSide::DOWN) {
        if ((static_cast<CoinBox *>(otherObject.get())->isWhileHit())) {
            this->getPosition()->jump(8);
        }
    }

    return TileTypes::MUSHROOM;//no interaction yet
}

bool Mushroom::waitingForDestroy() {
    return isDead(); //there is no case we are expecting removal
}

void Mushroom::step(long time __attribute((unused))) {
    if(directionChangeRequested) {
        directionRight = !directionRight;
        directionChangeRequested = false;
    }

    if (directionRight) {
        this->getPosition()->moveRight(1);
    } else {
        this->getPosition()->moveLeft(1);
    }

}
