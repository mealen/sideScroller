//
// Created by mesutcan on 11/20/18.
//

#include "Context.h"

Context::Context(std::shared_ptr<World> world, std::shared_ptr<Mario> player): world(world), player(player)  {

}

std::shared_ptr<World> Context::getWorld() const {
    return world;
}

std::shared_ptr<Mario> Context::getPlayer() const {
    return player;
}

int Context::getEndOfScreen() {
    return getPlayer()->getPosition()->getMaxRight() + (SCREEN_WIDTH / 2);
}
