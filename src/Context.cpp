//
// Created by mesutcan on 11/20/18.
//

#include "Context.h"

Context::Context(std::shared_ptr<World> world, std::shared_ptr<Mario> player): world(world), player(player)  {

}

World *Context::getWorld() const {
    return world.get();
}

Mario *Context::getPlayer() const {
    return player.get();
}
