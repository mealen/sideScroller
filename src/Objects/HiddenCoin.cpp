//
// Created by mesutcan on 11/20/18.
//

#include "HiddenCoin.h"
#include "../Utils.h"

AABB *HiddenCoin::getPosition() const {
    return this->collisionBox;
}

SDL_Texture *HiddenCoin::getTexture(long time) const {
    return this->texture[(time / (TOTAL_ANIM_TIME / 8)) % 4];
}

Map::TileTypes HiddenCoin::getTileType() const {
    return Map::TileTypes::EMPTY;
}

void HiddenCoin::render(SDL_Renderer *renderer, int x, int y, long time) {
    SDL_Rect screenPos;
    // calculate offset and add to x and y positions
    screenPos.x = collisionBox->getLeftBorder() - x + (((1 - SHRINK_FACTOR) * TILE_SIZE) / 2);
    screenPos.y = collisionBox->getUpBorder() - y + (((1 - SHRINK_FACTOR) * TILE_SIZE) / 2);
    // calculate width/height with shrink factor
    screenPos.w = TILE_SIZE * SHRINK_FACTOR;
    screenPos.h = TILE_SIZE * SHRINK_FACTOR;

    if(firstRenderTime == 0 ) {
        firstRenderTime = time;
    }

    long animTime = time - firstRenderTime;

    if(animTime >= TOTAL_ANIM_TIME) {
        isDestroyed = true;
    }
    float offset = sin(M_PI * (animTime) / TOTAL_ANIM_TIME);
    screenPos.y = screenPos.y - offset * 3 * TILE_SIZE;

    SDL_RenderCopyEx(renderer, getTexture(animTime), 0, &screenPos, 0, 0, SDL_FLIP_NONE);

}

Map::TileTypes HiddenCoin::interactWithSide(std::shared_ptr<Context> context,
                                            std::shared_ptr<InteractiveObject> otherObject, int interactionSide,
                                            long time) {
    return this->getTileType();
}

void HiddenCoin::step(long time) {

}

bool HiddenCoin::waitingForDestroy() {
    return isDestroyed;
}

HiddenCoin::HiddenCoin(SDL_Renderer *ren, int x, int y) {
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
