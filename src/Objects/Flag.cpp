//
// Created by engin on 24.04.2019.
//

#include "Flag.h"
#include "../Utils.h"


Flag::Flag(SDL_Renderer *ren, int x, int y) {
    collisionBox = new AABB(
            x * TILE_SIZE - TILE_SIZE, // 2 tiles width
            x * TILE_SIZE + TILE_SIZE -1,
            y * TILE_SIZE,
            y * TILE_SIZE + TILE_SIZE -1); //-1 because 32 is not part of box. pixels 0 - TILE_SIZE, TILE_SIZE excluded


    std::string flagImage = Utils::getResourcePath("flag") + "flag.bmp";
    texture = Utils::loadTexture(ren, flagImage);

}

Flag::~Flag() {
    SDL_DestroyTexture(texture);
    delete(collisionBox);
}
void Flag::render(SDL_Renderer *renderer, int x, int y, long time) {
    SDL_Rect screenPos;
    screenPos.x = collisionBox->getLeftBorder() - x;
    screenPos.y = collisionBox->getUpBorder() - y;
    screenPos.w = TILE_SIZE * 2;
    screenPos.h = TILE_SIZE;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, getTexture(time), nullptr, &screenPos, 0, nullptr, flip);
}

TileTypes Flag::interactWithSide(std::shared_ptr<Context> context, std::shared_ptr<InteractiveObject> otherObject,
                                 CollisionSide interactionSide, long time) {
    return EMPTY;
}

void Flag::step(std::shared_ptr<Context> context, long time) {

}


