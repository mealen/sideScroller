//
// Created by engin on 26.12.2018.
//

#include "HiddenCoinBox.h"
#include "Mario.h"
#include "Coin.h"
#include "../Context.h"

void HiddenCoinBox::step(std::shared_ptr<Context> context __attribute((unused)), long time) {
    if(isUsed) {
        return;
    }
    if (firstHitTime != 0 && firstHitTime + activeFor < time) {
        isUsed = true;
    }
}

TileTypes HiddenCoinBox::interactWithSide(std::shared_ptr<Context> context, std::shared_ptr<InteractiveObject> otherObject,
                                          CollisionSide interactionSide, long time) {
    if(otherObject->getTileType() == TileTypes::PLAYER && interactionSide == CollisionSide::DOWN) {
        if(isUsed) {
            //destroy logic
            if(breakTime == 0) {
                breakTime = time;
                whileHit = true;
            }
        } else {
            if(firstHitTime == 0) {
                firstHitTime = time;
            }
            lastHitTime = time;
            whileHit = true;
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
                    hiddenCoin->setStatic(false);
                    context->getWorld()->addObject(hiddenCoin);
                } else {
                    std::cerr << "Unexpected behavior. Player is not Mario. Who the f**K are you;" << std::endl;
                }
            }
        }
    }
    return TileTypes::HIDDEN_COIN_BOX;
}
