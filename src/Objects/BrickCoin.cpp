//
// Created by mesutcan on 11/20/18.
//

#include "BrickCoin.h"
#include "../Context.h"

int BrickCoin::interactWithSide(std::shared_ptr<Context> context, int interactionSide, long time) {
    if(hitTime != 0) {
        return 0;//if already interacted, don't allow again
    }
    std::cout << "interaction from side " << interactionSide << std::endl;
    if(interactionSide == 1) {
        //isDestroyed = true;
        for (uint32_t i = 0; i < breakSound.size(); ++i) {
            Mix_PlayChannel(-1, breakSound[i], 0);
        }

        hitTime = time;

        if (!isUsed) {
            Mario *mario = context->getPlayer();
            if (mario != nullptr) {
                mario->increaseCoin();
                HiddenCoin* hiddenCoin = new HiddenCoin(context->getWorld()->getRen(),
                        this->getPosition()->getLeftBorder() / TILE_SIZE,
                        (this->getPosition()->getUpBorder() / TILE_SIZE) - 1);
                context->getWorld()->addObject(hiddenCoin);
            } else {
                std::cerr << "Unexpected behavior. Player is not Mario. Who the f**K are you;" << std::endl;
            }

        }
    }


    return 0;//no interaction yet
};