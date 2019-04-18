//
// Created by mesutcan on 11/20/18.
//

#include "Coin.h"
#include "../Utils.h"
#include "Mario.h"

AABB *Coin::getPosition() const {
    return this->collisionBox;
}

SDL_Texture *Coin::getTexture(long time) const {
    return this->texture[(time / (TOTAL_ANIM_TIME / 8)) % 4];
}

TileTypes Coin::getTileType() const {
    if(isDestroyed) {
        return TileTypes::EMPTY;
    }
    return TileTypes::COIN;
}

void Coin::render(SDL_Renderer *renderer, int x, int y, long time) {
    SDL_Rect screenPos;
    // calculate offset and add to x and y positions
    screenPos.x = collisionBox->getLeftBorder() - x + (((1 - SHRINK_FACTOR) * TILE_SIZE) / 2);
    screenPos.y = collisionBox->getUpBorder() - y + (((1 - SHRINK_FACTOR) * TILE_SIZE) / 2);
    // calculate width/height with shrink factor
    screenPos.w = TILE_SIZE * SHRINK_FACTOR;
    screenPos.h = TILE_SIZE * SHRINK_FACTOR;

    if (isStatic) {

    } else {
        if (firstRenderTime == 0) {
            firstRenderTime = time;
        }

        long animTime = time - firstRenderTime;

        if (animTime >= TOTAL_ANIM_TIME) {
            isDestroyed = true;
        }
        float offset = sin(M_PI * (animTime) / TOTAL_ANIM_TIME);
        screenPos.y = screenPos.y - offset * 3 * TILE_SIZE;
    }

    SDL_RenderCopyEx(renderer, getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);

}
void Coin::die() {
    this->isDestroyed = true;
}

TileTypes Coin::interactWithSide(std::shared_ptr<Context> context __attribute((unused)),
                                            std::shared_ptr<InteractiveObject> otherObject __attribute((unused)), CollisionSide interactionSide __attribute((unused)),
                                            long time __attribute((unused))) {
    if(otherObject->getTileType() == TileTypes::PLAYER) {
        Mario *player = static_cast<Mario *>(otherObject.get());
        player->increaseCoin();
        die();
        return TileTypes::EMPTY;
    }

    return this->getTileType();
}

void Coin::step(std::shared_ptr<Context> context __attribute((unused)), long time  __attribute((unused))) {

}

bool Coin::waitingForDestroy() {
    return isDestroyed;
}

Coin::Coin(SDL_Renderer *ren, int x, int y) {
    collisionBox = new AABB(
            x * TILE_SIZE,
            x * TILE_SIZE + TILE_SIZE -1,
            y * TILE_SIZE,
            y * TILE_SIZE + TILE_SIZE -1);

    for (int i = 0; i < 4; i++) {
        std::string brickImage = Utils::getResourcePath("coin") + "coin_an" + std::to_string(i) + ".bmp";
        texture.push_back(Utils::loadTexture(ren, brickImage));
    }
}

void Coin::setStatic(bool isStatic) {
    this->isStatic = isStatic;
}