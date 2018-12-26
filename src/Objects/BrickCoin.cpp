//
// Created by mesutcan on 11/20/18.
//

#include "BrickCoin.h"
#include "../Context.h"

Map::TileTypes BrickCoin::interactWithSide(std::shared_ptr<Context> context,
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

        hitTime = time;

        if (!isUsed) {
            std::shared_ptr<Mario> mario = context->getPlayer();
            if (mario != nullptr) {
                mario->increaseCoin();
                std::shared_ptr<HiddenCoin> hiddenCoin = std::make_shared<HiddenCoin>(context->getWorld()->getRen(),
                        this->getPosition()->getLeftBorder() / TILE_SIZE,
                        (this->getPosition()->getUpBorder() / TILE_SIZE) - 1);
                context->getWorld()->addObject(hiddenCoin);
            } else {
                std::cerr << "Unexpected behavior. Player is not Mario. Who the f**K are you;" << std::endl;
            }

        }
    }


    return this->getTileType();//no interaction yet
};