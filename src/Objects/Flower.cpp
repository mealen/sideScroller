//
// Created by mesutcan on 11/20/18.
//

#include "Flower.h"
#include "../Context.h"

Flower::Flower(SDL_Renderer *ren, int x, int y) {//FIXME this should not need  renderer and map
    collisionBox = new AABB(
            x * TILE_SIZE,
            x * TILE_SIZE + TILE_SIZE -1,
            y * TILE_SIZE,
            y * TILE_SIZE + TILE_SIZE -1); //-1 because 32 is not part of box. pixels 0 - TILE_SIZE, TILE_SIZE excluded

    collisionBox->setPhysicsState(AABB::PhysicsState::DYNAMIC);


    for (int i = 0; i < 3; i++) {
        std::string brickImage = Utils::getResourcePath("flower") + "flower" + std::to_string(i) + ".bmp";
        texture.push_back(Utils::loadTexture(ren, brickImage));
    }

}

Flower::~Flower() {

    for (size_t i = 0; i < texture.size(); i++) {
        SDL_DestroyTexture(texture[i]);
    }

    delete(collisionBox);


}

TileTypes Flower::interactWithSide(std::shared_ptr<Context> context,
                                           std::shared_ptr<InteractiveObject> otherObject __attribute((unused)), CollisionSide interactionSide,
                                           long time) {
    if(hitTime != 0) {
        return TileTypes::FLOWER;//if already interacted, don't allow again
    }

    // if mario is coming from top, kill
    if(otherObject->getTileType() == TileTypes::PLAYER) {
        if(!otherObject->isDead()) {
            isSquashed = true;

            die(getTileType());
            Mario *player = static_cast<Mario *>(otherObject.get());
            player->setFire(true);
            return TileTypes::EMPTY;
        }
        // swap direction
    } else if (interactionSide == CollisionSide::RIGHT || interactionSide == CollisionSide::LEFT) {
        if(!directionChangeRequested) {
            directionRight = !directionRight;
        }
    }


    return TileTypes::FLOWER;//no interaction yet
}

void Flower::collideWithSide(std::shared_ptr<Context> context __attribute((unused)), TileTypes tile,
                               CollisionSide interactionSide, long time __attribute((unused))) {
    if (interactionSide == CollisionSide::DOWN) {

    }
    else if (interactionSide == CollisionSide::LEFT || interactionSide == CollisionSide::RIGHT || interactionSide == CollisionSide::INVALID) {
        if(tile != TileTypes::GROUND) {
            directionRight = !directionRight;
        }
    }

}


void Flower::setCreateTime(long time) {
    createTime = time;
}

bool Flower::waitingForDestroy() {
    return isDead(); //there is no case we are expecting removal
}


void Flower::step(std::shared_ptr<Context> context __attribute((unused)), long time __attribute((unused))) {

}


void Flower::render(SDL_Renderer* renderer, int x, int y, long time) {
    SDL_Rect screenPos;
    screenPos.x = collisionBox->getLeftBorder() - x;
    screenPos.y = collisionBox->getUpBorder() - y;
    screenPos.w = TILE_SIZE;
    screenPos.h = TILE_SIZE;

    SDL_RenderCopyEx(renderer, getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);

}


SDL_Texture* Flower::getTexture(long time __attribute((unused))) const {
    return texture[(time / 200) % 3];
}

AABB* Flower::getPosition() const {
    return collisionBox;
}

TileTypes Flower::getTileType() const {
    return TileTypes::FLOWER;
}