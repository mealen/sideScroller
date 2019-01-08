//
// Created by mesutcan on 11/20/18.
//

#include "MushroomBox.h"
#include "Mushroom.h"
#include "../Context.h"

TileTypes MushroomBox::interactWithSide(std::shared_ptr<Context> context,
                                           std::shared_ptr<InteractiveObject> otherObject __attribute((unused)), int interactionSide,
                                           long time) {
    if(hitTime != 0) {
        return this->getTileType();//if already interacted, don't allow again
    }
    if(interactionSide == 1) {
        //isDestroyed = true;
        for (uint32_t i = 0; i < breakSound.size(); ++i) {
            Mix_PlayChannel(-1, breakSound[i], 0);
        }
        if (!isUsed) {
            hitTime = time;
            std::shared_ptr<Mario> mario = context->getPlayer();
            if (mario != nullptr) {
                mushroomTemp = std::make_shared<Mushroom>(context->getWorld()->getRen(),
                                                          this->getPosition()->getLeftBorder() / TILE_SIZE,
                                                          (this->getPosition()->getUpBorder() / TILE_SIZE) - 1);
                worldTemp = context->getWorld();
            }
        }
    }
    return this->getTileType();//no interaction yet
};

void MushroomBox::render(SDL_Renderer* renderer, int x, int y, long time) {
    SDL_Rect screenPos;
    screenPos.x = collisionBox->getLeftBorder() - x;
    screenPos.y = collisionBox->getUpBorder() - y;
    screenPos.w = TILE_SIZE;
    screenPos.h = TILE_SIZE;
    if(hitTime != 0 && hitTime != time) {
        long animTime = time - hitTime;
        long mushroomTime = animTime;

        if(animTime > 200) {
            isUsed = true;
            animTime = 200;//stop animation after 500
        }
        float upSpeed = sin(M_PI * (animTime) / 200.0f);
        screenPos.y = collisionBox->getUpBorder() - y;//start from objects position
        screenPos.y = screenPos.y - upSpeed * 8;

        if(!mushroomCreated) {
            if (mushroomTime > 500) {
                worldTemp->addObject(mushroomTemp);
                worldTemp = nullptr;
                mushroomTemp = nullptr;
                mushroomCreated = true;
            } else {
                float upSpeed = 32 * mushroomTime / 500;
                SDL_Rect mushroomPos = screenPos;
                if(mushroomPos.y > screenPos.y - upSpeed) {
                    mushroomPos.y = screenPos.y - upSpeed;
                }
                SDL_RenderCopyEx(renderer, mushroomTemp->getTexture(time), 0, &mushroomPos, 0, 0, SDL_FLIP_NONE);
            }
        }


        SDL_RenderCopyEx(renderer, getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);

    } else {
        SDL_RenderCopyEx(renderer, getTexture(time), 0, &screenPos, 0, 0, SDL_FLIP_NONE);
    }
};