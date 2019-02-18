//
// Created by mesutcan on 11/20/18.
//

#ifndef MARIO_CONTEXT_H
#define MARIO_CONTEXT_H


#include "World.h"

class Context {
public:
    Context(std::shared_ptr<World> world, std::shared_ptr<Mario> player);

    std::shared_ptr<World> getWorld() const;

    std::shared_ptr<Mario> getPlayer() const;

    int getEndOfScreen();


private:
    std::shared_ptr<World> world;
    std::shared_ptr<Mario> player;


};


#endif //MARIO_CONTEXT_H
