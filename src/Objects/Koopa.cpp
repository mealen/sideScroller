//
// Created by engin on 5.02.2019.
//

#include "Koopa.h"
#include "../Utils.h"
#include "CoinBox.h"
#include "Brick.h"
#include <cmath>



Koopa::Koopa(SDL_Renderer *ren, int x, int y) {//FIXME this should not need  renderer and map
    collisionBox = new AABB(
            x * TILE_SIZE,
            x * TILE_SIZE + TILE_SIZE -1,
            y * TILE_SIZE,
            y * TILE_SIZE + TILE_SIZE -1); //-1 because 32 is not part of box. pixels 0 - TILE_SIZE, TILE_SIZE excluded

    collisionBox->setPhysicsState(AABB::PhysicsState::DYNAMIC);
    for (int i = 0; i < 2; i++) {
        std::string koopaImage = Utils::getResourcePath("koopa") + "koopa_" + std::to_string(i) + ".bmp";
        texture.push_back(Utils::loadTexture(ren, koopaImage));
    }

    std::string koopaImage = Utils::getResourcePath("koopa") + "koopa_dead.bmp";
    texture.push_back(Utils::loadTexture(ren, koopaImage));

    deadSound.push_back(Mix_LoadWAV("./res/sounds/stomp.wav"));
}

Koopa::~Koopa() {
    for (size_t i = 0; i < texture.size(); i++) {
        SDL_DestroyTexture(texture[i]);
    }
    delete(collisionBox);
}

SDL_Texture* Koopa::getTexture(long time) const {
    if (killHitTime != 0) {
        return texture[2];
    }

    if (this->isHideInShell) {
        return texture[2];
    }

    return texture[(time / 250) % 2];
}

AABB* Koopa::getPosition() const {
    return collisionBox;
}

TileTypes Koopa::getTileType() const {
    if (killHitTime != 0) {
        return TileTypes::EMPTY;
    } else {
        return TileTypes::KOOPA;
    }
}

void Koopa::render(SDL_Renderer* renderer, int x, int y, long time) {
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (x + SCREEN_WIDTH < collisionBox->getLeftBorder() && directionRight) {
        die(TileTypes::OUT_OF_MAP);
    }

    if (directionRight) {
        flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_HORIZONTAL);
    }
    SDL_Rect screenPos;
    int yOffset=0;
    if(!isHideInShell && killHitTime == 0) {
        yOffset = (int)std::floor(TILE_SIZE * 0.5f);
    }
    screenPos.x = collisionBox->getLeftBorder() - x;
    screenPos.y = collisionBox->getUpBorder() - y - yOffset;
    screenPos.w = TILE_SIZE;
    screenPos.h = TILE_SIZE + yOffset;

    if (killHitTime != 0) {
        flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_VERTICAL);
        if (collisionBox->getUpBorder() > SCREEN_HEIGHT) {
            isRemoveWaiting = true;
        }
    }

    SDL_RenderCopyEx(renderer, getTexture(time), 0, &screenPos, 0, 0, flip);

}

void Koopa::collideWithSide(std::shared_ptr<Context> context __attribute((unused)), TileTypes tile,
                             CollisionSide interactionSide, long time __attribute((unused))) {
    if (interactionSide == CollisionSide::DOWN) {

    } else if (interactionSide == CollisionSide::LEFT || interactionSide == CollisionSide::RIGHT || interactionSide == CollisionSide::INVALID) {
        directionRight = !directionRight;
    }

}

TileTypes Koopa::interactWithSide(std::shared_ptr<Context> context __attribute((unused)), std::shared_ptr<InteractiveObject> otherObject,
                                   CollisionSide interactionSide, long time) {
    // if mario is coming from top, kill
    if(interactionSide == CollisionSide::UP && otherObject->getTileType() == TileTypes::PLAYER) {
        if(!otherObject->isDead()) {
            if(isShellMoving) {
                isShellMoving = false;
                hideInShellTime = time;
            } else {
                if(isHideInShell) {
                    interactionSide = CollisionSide::RIGHT;
                    if (otherObject->getPosition()->getLeftBorder() < this->getPosition()->getLeftBorder()){
                        interactionSide = CollisionSide::LEFT;
                    }
                    moveShell(interactionSide);
                } else {
                    isHideInShell = true;
                    hideInShellTime = time;
                    die(getTileType());
                }

            }
            otherObject->getPosition()->setUpwardSpeed(Mario::JUMP_SPEED / 2);

            for (uint32_t i = 0; i < deadSound.size(); ++i) {
                Mix_PlayChannel(-1, deadSound[i], 0);
            }

            return TileTypes::EMPTY;
        }
        // swap direction
    } else if (interactionSide == CollisionSide::LEFT || interactionSide == CollisionSide::RIGHT) {
        if(isShellMoving) {
            otherObject->die(getTileType());
        } else {
            if (otherObject->getTileType() == TileTypes::PLAYER) {
                if (isHideInShell) {
                    moveShell(interactionSide);
                } else {
                    otherObject->die(getTileType());
                }
            } else {
                if (!directionChangeRequested) {
                    directionRight = !directionRight;
                    if (otherObject->getTileType() == TileTypes::KOOPA) {
                        Koopa *otherKoopa = static_cast<Koopa *>(otherObject.get());
                        otherKoopa->directionChangeRequested = true;
                    }
                }

            }
        }
    }

    if (otherObject->getTileType() == TileTypes::FIREBALL) {
        this->killHitTime = time;
        collisionBox->setPhysicsState(AABB::NON_INTERACTIVE);
        collisionBox->setUpwardSpeed(8);
        collisionBox->setUpBorder(collisionBox->getUpBorder() + TILE_SIZE / 4);
        collisionBox->setDownBorder(collisionBox->getDownBorder() + TILE_SIZE / 4);
    }

    if (otherObject->getTileType() == TileTypes::PLAYER) {
        if ((static_cast<Mario *>(otherObject.get())->getStar())) {
            this->killHitTime = time;
            collisionBox->setPhysicsState(AABB::NON_INTERACTIVE);
            collisionBox->setUpwardSpeed(8);
            collisionBox->setUpBorder(collisionBox->getUpBorder() + TILE_SIZE/4);
            collisionBox->setDownBorder(collisionBox->getDownBorder() + TILE_SIZE/4);
        }
    }


    if (otherObject->getTileType() == TileTypes::BRICK && interactionSide == CollisionSide::DOWN) {
        if ((static_cast<Brick *>(otherObject.get())->isWhileHit())) {
            this->killHitTime = time;
            collisionBox->setPhysicsState(AABB::NON_INTERACTIVE);
            collisionBox->setUpwardSpeed(8);
            collisionBox->setUpBorder(collisionBox->getUpBorder() + TILE_SIZE/4);
            collisionBox->setDownBorder(collisionBox->getDownBorder() + TILE_SIZE/4);
        }
    }

    if (otherObject->getTileType() == TileTypes::COIN_BOX && interactionSide == CollisionSide::DOWN) {
        if ((static_cast<CoinBox *>(otherObject.get())->isWhileHit())) {
            this->killHitTime = time;
            collisionBox->setPhysicsState(AABB::NON_INTERACTIVE);
            collisionBox->setUpwardSpeed(8);
            collisionBox->setUpBorder(collisionBox->getUpBorder() + TILE_SIZE/4);
            collisionBox->setDownBorder(collisionBox->getDownBorder() + TILE_SIZE/4);
        }
    }

    return TileTypes::KOOPA;//no interaction yet
}

void Koopa::moveShell(const CollisionSide &interactionSide) {
    isShellMoving = true;
    if (interactionSide == CollisionSide::LEFT) {
        collisionBox->moveRight(5);
        directionRight = true;
    } else if (interactionSide == CollisionSide::RIGHT) {
        collisionBox->moveLeft(5);
        directionRight = false;
    }
}

bool Koopa::waitingForDestroy() {
    return isRemoveWaiting; //there is no case we are expecting removal
}

void Koopa::step(std::shared_ptr<Context> context __attribute((unused)), long time) {
    if (killHitTime != 0) {
        return;
    }
    if(directionChangeRequested) {
        directionRight = !directionRight;
        directionChangeRequested = false;
    }
    if(!isHideInShell) {
        if (directionRight) {
            this->getPosition()->moveRight(1);
        } else {
            this->getPosition()->moveLeft(1);
        }
    }
    if(isShellMoving) {
        if (directionRight) {
            this->getPosition()->moveRight(5);
        } else {
            this->getPosition()->moveLeft(5);
        }
    }
    if(hideInShellTime != 0 && !isShellMoving && time - hideInShellTime >= 2500) {
        isHideInShell = false;
        hideInShellTime = 0;
    }
}

void Koopa::die(TileTypes type) {
    if (type == TileTypes::OUT_OF_MAP) {
        this->isRemoveWaiting = true;
    }
}

bool Koopa::getShellMoving() const {
    return isShellMoving;
}

