//
// Created by engin on 26.12.2018.
//

#include "HiddenCoinBox.h"
#include "Mario.h"
#include "Coin.h"
#include "../Context.h"

void HiddenCoinBox::step(long time) {
    if(isUsed) {
        return;
    }
    if (firstHitTime != 0 && firstHitTime + activeFor < time) {
        isUsed = true;
    }
}

Map::TileTypes
HiddenCoinBox::interactWithSide(std::shared_ptr<Context> context, std::shared_ptr<InteractiveObject> otherObject,
                                int interactionSide, long time) {
    if(otherObject->getTileType() == Map::PLAYER && interactionSide == 1) {
        if(isUsed) {
            //destroy logic
            if(breakTime == 0) {
                breakTime = time;
            }
        } else {
            if(firstHitTime == 0) {
                firstHitTime = time;
            }
            lastHitTime = time;
            // coin pickup


            for (uint32_t i = 0; i < breakSound.size(); ++i) {
                Mix_PlayChannel(-1, breakSound[i], 0);
            }

            if (!isUsed) {
                std::shared_ptr<Mario> mario = context->getPlayer();
                if (mario != nullptr) {
                    mario->increaseCoin();
                    std::shared_ptr<Coin> hiddenCoin = std::make_shared<Coin>(context->getWorld()->getRen(),
                                                                              this->getPosition()->getLeftBorder() / TILE_SIZE,
                                                                              (this->getPosition()->getUpBorder() / TILE_SIZE) - 1);
                    context->getWorld()->addObject(hiddenCoin);
                } else {
                    std::cerr << "Unexpected behavior. Player is not Mario. Who the f**K are you;" << std::endl;
                }
            }
        }
    }
    return Map::HIDDEN_COIN_BOX;
}
