//
// Created by engin on 24.04.2019.
//

#include "Flag.h"
#include "../Utils.h"
#include "../World.h"
#include "../Context.h"
/**
 * the are 2 objects here flag and pole.
 * Flag is not interactive, so it doesn't have an AABB. Its placement and movement will be governed manually by this class
 * pole is 10 tiles long, but half tile wide.
 *
 * Interaction logic:
 * Any interaction with player should check the interaction height
 * give points based on height
 * make player kinematic
 * switch player state to POLE
 * move player down to base with speed POLE_MOVEMENT_SPEED
 * move flag to base with speed POLE_MOVEMENT_SPEED
 * wait until both animation finishes
 * move player to the castle door using walking animation
 * trigger level change
 *
 * @param ren
 * @param x
 * @param y
 */
Flag::Flag(SDL_Renderer *ren, int x, int y) {
    collisionBox = new AABB(
            x * TILE_SIZE + TILE_SIZE /4, // 1/2 tiles width
            x * TILE_SIZE + TILE_SIZE - TILE_SIZE / 4 -1,
            y * TILE_SIZE,
            y * TILE_SIZE + TILE_SIZE *10 -1); //10 long. -1 because 32 is not part of box. pixels 0 - TILE_SIZE, TILE_SIZE excluded


    std::string flagImage = Utils::getResourcePath("flag") + "flag.bmp";
    texture = Utils::loadTexture(ren, flagImage);

}

Flag::~Flag() {
    SDL_DestroyTexture(texture);
    delete(collisionBox);
}
void Flag::render(SDL_Renderer *renderer, int x, int y, long time) {
    SDL_Rect screenPos;
    screenPos.x = collisionBox->getLeftBorder() - ((float)(TILE_SIZE) * (5.0/4.0)) - x;
    screenPos.y = collisionBox->getUpBorder() - y;
    screenPos.w = TILE_SIZE * 2;
    screenPos.h = TILE_SIZE;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, getTexture(time), nullptr, &screenPos, 0, nullptr, flip);
}

TileTypes Flag::interactWithSide(std::shared_ptr<Context> context, std::shared_ptr<InteractiveObject> otherObject,
                                 CollisionSide interactionSide, long time) {
    if(!this->activated) {
        std::cout << "flag collision at position " << otherObject->getPosition()->getUpBorder() << std::endl;
        this->activated = true;
        std::shared_ptr<World::PlayerAnimation> playerAnimation = std::make_shared<World::PlayerAnimation>();
        playerAnimation->movementSide = World::Sides::DOWN;
        playerAnimation->maximumMovement = collisionBox->getDownBorder() - otherObject->getPosition()->getDownBorder();
        playerAnimation->movementSpeed = 2;
        playerAnimation->duration = playerAnimation->maximumMovement / playerAnimation->movementSpeed * 1000/60;
        context->getWorld()->playAnimation(playerAnimation, Mario::TextureNames::POLE);
    }
    return EMPTY;
}

void Flag::step(std::shared_ptr<Context> context, long time) {

}


