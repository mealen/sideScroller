//
// Created by mesutcan on 11/20/18.
//

#include "CoinBox.h"
#include "../Context.h"

    TileTypes CoinBox::interactWithSide(std::shared_ptr<Context> context,
                                           std::shared_ptr<InteractiveObject> otherObject __attribute((unused)), CollisionSide interactionSide,
                                           long time) {
    if(hitTime != 0) {
        return this->getTileType();//if already interacted, don't allow again
    }
    if(interactionSide == CollisionSide::DOWN) {
        //isDestroyed = true;
        for (uint32_t i = 0; i < breakSound.size(); ++i) {
            if(Mix_PlayChannel(-1, breakSound[i], 0)==-1) {
                printf("Mix_PlayChannel: %s\n",Mix_GetError());
                // may be critical error, or maybe just no channels were free.
                // you could allocated another channel in that case...
            }
        }

        hitTime = time;
        this->whileHit = true;

        if (!isUsed) {
            std::shared_ptr<Mario> mario = context->getPlayer();
            if (mario != nullptr) {
                mario->increaseCoin();
                std::shared_ptr<Coin> hiddenCoin = std::make_shared<Coin>(context->getWorld()->getRen(),
                        this->getPosition()->getLeftBorder() / TILE_SIZE,
                        (this->getPosition()->getUpBorder() / TILE_SIZE) - 1, false);
                context->getWorld()->addObject(hiddenCoin);
            } else {
                std::cerr << "Unexpected behavior. Player is not Mario. Who the f**K are you;" << std::endl;
            }
        }
    }


    return this->getTileType();//no interaction yet
}