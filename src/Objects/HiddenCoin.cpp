//
// Created by mesutcan on 11/20/18.
//

#include "HiddenCoin.h"
#include "../Utils.h"

AABB *HiddenCoin::getPosition() const {
    return this->collisionBox;
}

SDL_Texture *HiddenCoin::getTexture(long time) const {
    return this->texture[(time / 248) % 4];
}

Map::TileTypes HiddenCoin::getTileType() const {
    return Map::TileTypes::EMPTY;
}

void HiddenCoin::render(SDL_Renderer *renderer, int x, int y, long time) {
    SDL_Rect screenPos;
    screenPos.x = collisionBox->getLeftBorder() - x;
    screenPos.y = collisionBox->getUpBorder() - y;
    screenPos.w = TILE_SIZE-5;
    screenPos.h = TILE_SIZE-5;

    if(firstRenderTime == 0 ) {
        firstRenderTime = time;
    }

    long animTime = time - firstRenderTime;

    if(animTime >= 1000) {
        isDestroyed = true;
    }
    float offset = sin(M_PI * (animTime) / 1000.0f);
    screenPos.y = screenPos.y - offset * 3 * TILE_SIZE;

    SDL_RenderCopyEx(renderer, getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);

}

int HiddenCoin::interactWithSide(std::shared_ptr<Context> context, int interactionSide, long time) {
    return 0;
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
