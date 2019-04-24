//
// Created by engin on 11.11.2017.
//

#include <fstream>
#include <iostream>

#include "World.h"
#include "Context.h"
#include "Constants.h"
#include "Objects/Brick.h"
#include "Objects/CoinBox.h"
#include "Objects/Goomba.h"
#include "Objects/MushroomBox.h"
#include "Objects/Koopa.h"
#include "Objects/StarBrick.h"
#include "Objects/HiddenCoinBox.h"
#include "InputHandler.h"
#include "Objects/Flag.h"

TileTypes World::collide(int rightSpeed, int downSpeed, long time, std::shared_ptr<Context> context,
                              std::shared_ptr<InteractiveObject> interactiveObject) {
    TileTypes staticTile = TileTypes::EMPTY;
    //we need 4 checks, since at any given time, object can be at 4 different places.

    int horCheck, horCheck2, verCheck, verCheck2;
    bool checkDoubleVer = false; //these flags is used for 0 speed checks
    bool checkDoubleHor = false; //these flags is used for 0 speed checks
    CollisionSide collisionWithStaticSide = CollisionSide::INVALID;
    if(rightSpeed > 0) {
        horCheck = interactiveObject->getPosition()->getRightBorder() + rightSpeed;
    } else if (rightSpeed < 0){
        horCheck = interactiveObject->getPosition()->getLeftBorder() + rightSpeed;
    } else {
        horCheck = interactiveObject->getPosition()->getLeftBorder();
        horCheck2 = interactiveObject->getPosition()->getRightBorder();
        checkDoubleHor = true;
    }

    if(downSpeed >0) {
        verCheck = interactiveObject->getPosition()->getDownBorder() + downSpeed;
    } else if (downSpeed < 0){
        verCheck = interactiveObject->getPosition()->getUpBorder() + downSpeed;
    } else {
        verCheck = interactiveObject->getPosition()->getUpBorder();
        verCheck2 = interactiveObject->getPosition()->getDownBorder();
        checkDoubleVer = true;
    }
    staticTile = getTileObject(horCheck/TILE_SIZE, verCheck/TILE_SIZE);
    if(checkDoubleVer) {
        staticTile = std::max(staticTile, getTileObject(horCheck/TILE_SIZE, verCheck2/TILE_SIZE));
    }
    if(checkDoubleHor) {
        staticTile = std::max(staticTile, getTileObject(horCheck2/TILE_SIZE, verCheck/TILE_SIZE));
    }
    if(checkDoubleHor && checkDoubleVer){
        staticTile = std::max(staticTile, getTileObject(horCheck2/TILE_SIZE, verCheck2/TILE_SIZE));
    }

    if(interactiveObject->getTileType() == TileTypes::PLAYER) {
        //std::cout << "tile type " << tile << std::endl;
    }
    if(staticTile != TileTypes::EMPTY && staticTile != TileTypes::OUT_OF_MAP) {
        if (getTileObject(horCheck / TILE_SIZE, (verCheck - downSpeed) / TILE_SIZE) != TileTypes::EMPTY) {
            if (rightSpeed >= 0) {
                collisionWithStaticSide = CollisionSide::RIGHT;
            } else {
                collisionWithStaticSide = CollisionSide::LEFT;
            }
        } else {
            if (downSpeed >= 0) {
                collisionWithStaticSide = CollisionSide::DOWN;
            } else {
                collisionWithStaticSide = CollisionSide::UP;
            }
        }
    }

    //this tile is the tile that is not interactive object
    TileTypes tile = TileTypes::EMPTY;
    std::shared_ptr<InteractiveObject> collidingObject = nullptr;
    std::shared_ptr<InteractiveObject> testingObject = nullptr;
    CollisionSide collisionSide = CollisionSide::INVALID;
    for (unsigned int i = 0; i < objects.size(); ++i) {
        if (objects[i] == interactiveObject) {
            continue;
        }
        // that 4 finds if that object collide with caller
        if (interactiveObject->getPosition()->getUpBorder() + downSpeed > objects[i]->getPosition()->getDownBorder()) {
            continue;
        }

        if (interactiveObject->getPosition()->getDownBorder() + downSpeed < objects[i]->getPosition()->getUpBorder()) {
            continue;
        }

        if (interactiveObject->getPosition()->getRightBorder() + rightSpeed <
            objects[i]->getPosition()->getLeftBorder()) {
            continue;
        }

        if (interactiveObject->getPosition()->getLeftBorder() + rightSpeed >
            objects[i]->getPosition()->getRightBorder()) {
            continue;
        }
        if(objects[i]->getTileType() == TileTypes::COIN) {
            std::cout << "found object to collide with type:" << objects[i]->getTileType() << std::endl;
        }
        // collision are prioritized as following 1 = 2 > 3 = 4
        //this part makes sure colliding object has the highest priority by tile type
        if (tile <= objects[i]->getTileType() && (objects[i]->getTileType() != TileTypes::EMPTY && objects[i]->getPosition()->getPhysicsState() != AABB::PhysicsState::NON_INTERACTIVE)) {

            testingObject = objects[i];
            //now we know there is a collision, check what is the direction of collision
            if (interactiveObject->getPosition()->getUpBorder() >= objects[i]->getPosition()->getDownBorder()) {
                collidingObject = testingObject;
                tile = objects[i]->getTileType();
                collisionSide = CollisionSide::DOWN;

            }
            if (interactiveObject->getPosition()->getDownBorder() <= objects[i]->getPosition()->getUpBorder()) {
                collidingObject = testingObject;
                tile = objects[i]->getTileType();
                collisionSide = CollisionSide::UP;
            }

            if (interactiveObject->getPosition()->getRightBorder() <= objects[i]->getPosition()->getLeftBorder()) {
                if (collisionSide == CollisionSide::LEFT || collisionSide == CollisionSide::RIGHT || collisionSide == CollisionSide::INVALID) {
                    tile = objects[i]->getTileType();
                    collisionSide = CollisionSide::LEFT;
                    collidingObject = testingObject;
                }
            }

            if (interactiveObject->getPosition()->getLeftBorder() >= objects[i]->getPosition()->getRightBorder()) {
                if (collisionSide == CollisionSide::LEFT || collisionSide == CollisionSide::RIGHT || collisionSide == CollisionSide::INVALID) {
                    tile = objects[i]->getTileType();
                    collisionSide = CollisionSide::RIGHT;
                    collidingObject = testingObject;
                }
            }
            if (collidingObject == nullptr) {
                //this means we are not colliding by side, but we are inside
                tile = objects[i]->getTileType();
                collidingObject = testingObject;
                collisionSide = CollisionSide ::INVALID;

            }
        }
    }

    if (collidingObject == interactiveObject) {
        return TileTypes::EMPTY;
    }
    if (collidingObject != NULL && (interactiveObject->getPosition()->getPhysicsState() != AABB::PhysicsState::NON_INTERACTIVE &&
                                    collidingObject->getPosition()->getPhysicsState() != AABB::PhysicsState::NON_INTERACTIVE)) {
        tile = collidingObject->interactWithSide(context, interactiveObject, collisionSide, time);
        CollisionSide reverseCollisionSide;
        switch (collisionSide) {
            case CollisionSide::DOWN:
                reverseCollisionSide = CollisionSide::UP;
                break;
            case CollisionSide::UP:
                reverseCollisionSide = CollisionSide::DOWN;
                break;
            case CollisionSide::LEFT:
                reverseCollisionSide = CollisionSide::RIGHT;
                break;
            case CollisionSide::RIGHT:
                reverseCollisionSide = CollisionSide::LEFT;
                break;
            case CollisionSide::INVALID:
                reverseCollisionSide = CollisionSide::INVALID;
        }
        interactiveObject->interactWithSide(context, collidingObject, reverseCollisionSide, time);
    }
    //collide with static world even if collided with an object
    if (staticTile != TileTypes::EMPTY && staticTile != TileTypes::OUT_OF_MAP) {
        interactiveObject->collideWithSide(context, staticTile, collisionWithStaticSide,
                                           time);
    }

    if(tile == TileTypes::EMPTY || tile == TileTypes::OUT_OF_MAP) {
        return staticTile;
    }
    return tile;
}

void World::stepSimulation(long time, std::shared_ptr<Context> context) {
    int middleOfScreenPixel = SCREEN_WIDTH / 2;

    if(portalTriggeredExternally) {
        if ((time - portalAnimationStartTime) >= portalAnimationDuration) {
            mario->getPosition()->setPhysicsState(AABB::PhysicsState::DYNAMIC);
            portalAnimationStartTime = 0;
            portalTriggeredExternally = false;
        } else {
            if(animatePortal(time)) {
                //don't wait if animation finished
                mario->getPosition()->setPhysicsState(AABB::PhysicsState::DYNAMIC);
                portalAnimationStartTime = 0;
                portalTriggeredExternally = false;
            } else {
                return;
            }
        }
    }
    for (size_t i = 0; i < this->objects.size(); ++i) {
        if (context->getPlayer()->getPosition()->getMaxRight() + middleOfScreenPixel + TILE_SIZE >
            this->objects[i]->getPosition()->getLeftBorder()) {

            if ((context->getPlayer()->isFireAnimationStarted() || context->getPlayer()->isGrowStarted()) &&
                this->objects[i]->getTileType() != TileTypes::PLAYER) {
                continue;
            }
            this->objects[i]->step(context, time);
        }
    }

    for (size_t i = 0; i < this->objects.size(); ++i) {
        std::shared_ptr<InteractiveObject> interactiveObject = this->objects[i];
        if ((context->getPlayer()->isFireAnimationStarted() || context->getPlayer()->isGrowStarted()) &&
            interactiveObject->getTileType() != TileTypes::PLAYER) {
            continue;
        }
        stepSingleObject(time, context, interactiveObject);
    }

    for (unsigned int i = 0; i < objects.size(); i++) {
        if (objects[i]->waitingForDestroy()) {
            objects.erase(objects.begin() + i);
        }
    }
}

void World::stepSingleObject(long time, const std::shared_ptr<Context> &context,
                      const std::shared_ptr<InteractiveObject> interactiveObject) {
    AABB *aabb = interactiveObject->getPosition();

    if(aabb->getPhysicsState() == AABB::PhysicsState::STATIC) {
        return;
    }

    if (aabb->getPhysicsState() == AABB::PhysicsState::DYNAMIC && mario.get()->isKilled()) {
        return;
    }

    int horizontalSpeed = aabb->getHorizontalSpeed();

    TileTypes tile = collide(horizontalSpeed, 0, time, context, interactiveObject);

    if ((tile == TileTypes::EMPTY || tile == TileTypes::OUT_OF_MAP) || aabb->getPhysicsState() != AABB::DYNAMIC) {
        aabb->setLeftBorder(aabb->getLeftBorder() + horizontalSpeed);
        aabb->setRightBorder(aabb->getRightBorder() + horizontalSpeed);
    } else if(abs(horizontalSpeed) > 1) {
        // if something is moving faster than 1, it is possible that object we hit is further than 1, find out the
        // distance, and move that amount
        int checkStep = -1;
        if(horizontalSpeed < 0) {
            checkStep = 1;
        }
        while(abs(horizontalSpeed) > 1) {
            horizontalSpeed += checkStep;
            tile = collide(horizontalSpeed, 0, time, context, interactiveObject);
            if(tile == TileTypes::EMPTY) {
                //Found the empty distance
                aabb->setLeftBorder(aabb->getLeftBorder() + horizontalSpeed);
                aabb->setRightBorder(aabb->getRightBorder() + horizontalSpeed);
                break;
            }
        }
    }
    
    aabb->setHorizontalSpeed(0);

    if (aabb->isHasJumpTriggered()) {
        aabb->setHasJumpTriggered(false);
        tile = collide(0, 1, time, context, interactiveObject);

        if (tile != TileTypes::EMPTY && aabb->getPhysicsState() == AABB::DYNAMIC) {
            aabb->setUpwardSpeed(aabb->getUpwardRequest());
        }
        aabb->setUpwardRequest(0);
    }
    int upwardSpeed = aabb->getUpwardSpeed();
    tile = collide(0, -1 * upwardSpeed, time, context, interactiveObject);
    //check if moving with upward speed is possible
    if (tile == TileTypes::OUT_OF_MAP) {
        if (aabb->getUpwardSpeed() < 0) {
            // mario dies
            interactiveObject->die(tile);
        }
    }
    if(aabb->getPhysicsState() == AABB::DYNAMIC || aabb->getPhysicsState() == AABB::NON_INTERACTIVE) {
        if ((tile != TileTypes::EMPTY &&
             tile != TileTypes::OUT_OF_MAP) && aabb->getPhysicsState() == AABB::DYNAMIC) {//if not possible, match the tile, and then stop
            int curSize = (aabb->getDownBorder() - aabb->getUpBorder());
            aabb->setUpBorder(aabb->getUpBorder() - upwardSpeed);
            if (aabb->getUpwardSpeed() > 0) {
                aabb->setUpBorder(((aabb->getUpBorder() + TILE_SIZE) / TILE_SIZE) * TILE_SIZE);
            } else {
                aabb->setUpBorder((aabb->getUpBorder() / TILE_SIZE) * TILE_SIZE);
            }
            aabb->setDownBorder(aabb->getUpBorder() + curSize);
            aabb->setUpwardSpeed(0);
            aabb->setHasJumped(false);
        } else { //if possible update
            aabb->setUpBorder(aabb->getUpBorder() - aabb->getUpwardSpeed());
            aabb->setDownBorder(aabb->getDownBorder() - aabb->getUpwardSpeed());
            aabb->setUpwardSpeed(aabb->getUpwardSpeed() - 1);
        }
    }
}

void World::load(std::string worldName, int &error, Mix_Music *&music) {
    this->music = music;
    std::string logicFile = Utils::getResourcePath("levels") + worldName + "_logic.txt";
    std::string worldBackgroundImagePath = Utils::getResourcePath("levels") + worldName + "_bg.bmp";
    if(loadTexture(ren, worldBackgroundImageTexture, mapWidth, worldBackgroundImagePath) != 0) {
        error = 1;
        return;
    }
    std::string worldForegroundImagePath = Utils::getResourcePath("levels") + worldName + "_fg.bmp";
    worldForegroundImageTexture = Utils::loadTexture(ren, worldForegroundImagePath);
    if(worldForegroundImageTexture == nullptr) {
        error = 1;
        return;
    }
    std::ifstream mapFile(logicFile);
    std::string line;
    if (mapFile.is_open()) {
        int lineNumber = 0;
        while (getline(mapFile, line)) {
            //std::cout << line << '\n';

            for (unsigned int i = 0; i < line.length(); i++) {
                tiles[i][lineNumber] = (TileTypes) (line.at(i) -
                                                    '0');//this removes char 0 to make ascii values match tile numbers
            }
            lineNumber++;
            if (lineNumber > 14) {
                break;
            }
        }
        parseAdvancedFeatures(mapFile);
        mapFile.close();
    } else {
        error = 1;
        return;
    }

    std::shared_ptr<Brick> brick;
    SDL_Rect brickPos = getAndRemoveObject(TileTypes::BRICK);
    while (brickPos.x != -1 && brickPos.y != -1) {
        brick = std::make_shared<Brick>(ren, brickPos.x, brickPos.y);
        addObject(brick);
        brickPos = getAndRemoveObject(TileTypes::BRICK);
    }

    std::shared_ptr<CoinBox> brickCoin;
    SDL_Rect brickCoinPos = getAndRemoveObject(TileTypes::COIN_BOX);
    while (brickCoinPos.x != -1 && brickCoinPos.y != -1) {
        brickCoin = std::make_shared<CoinBox>(ren, brickCoinPos.x, brickCoinPos.y);
        addObject(brickCoin);
        brickCoinPos = getAndRemoveObject(TileTypes::COIN_BOX);
    }

    std::shared_ptr<Goomba> goomba;
    SDL_Rect goombaPos = getAndRemoveObject(TileTypes::GOOMBA);
    while (goombaPos.x != -1 && goombaPos.y != -1) {
        goomba = std::make_shared<Goomba>(ren, goombaPos.x, goombaPos.y);
        addObject(goomba);
        goombaPos = getAndRemoveObject(TileTypes::GOOMBA);
    }

    std::shared_ptr<Koopa> koopa;
    SDL_Rect koopaPos = getAndRemoveObject(TileTypes::KOOPA);
    while (koopaPos.x != -1 && koopaPos.y != -1) {
        koopa = std::make_shared<Koopa>(ren, koopaPos.x, koopaPos.y);
        addObject(koopa);
        koopaPos = getAndRemoveObject(TileTypes::KOOPA);
    }

    std::shared_ptr<MushroomBox> brickMushroom;
    SDL_Rect brickMushroomPos = getAndRemoveObject(TileTypes::MUSHROOM_BOX);
    while (brickMushroomPos.x != -1 && brickMushroomPos.y != -1) {
        brickMushroom = std::make_shared<MushroomBox>(ren, brickMushroomPos.x, brickMushroomPos.y);
        addObject(brickMushroom);
        brickMushroomPos = getAndRemoveObject(TileTypes::MUSHROOM_BOX);
    }

    std::shared_ptr<HiddenCoinBox> hiddenCoinBox;
    SDL_Rect hiddenCoinBoxPos = getAndRemoveObject(TileTypes::HIDDEN_COIN_BOX);
    while (hiddenCoinBoxPos.x != -1 && hiddenCoinBoxPos.y != -1) {
        hiddenCoinBox = std::make_shared<HiddenCoinBox>(ren, hiddenCoinBoxPos.x, hiddenCoinBoxPos.y);
        addObject(hiddenCoinBox);
        hiddenCoinBoxPos = getAndRemoveObject(TileTypes::HIDDEN_COIN_BOX);
    }

    std::shared_ptr<StarBrick> starBrick;
    SDL_Rect starBrickPos = getAndRemoveObject(TileTypes::STAR_BRICK);
    while (starBrickPos.x != -1 && starBrickPos.y != -1) {
        starBrick = std::make_shared<StarBrick>(ren, starBrickPos.x, starBrickPos.y);
        addObject(starBrick);
        starBrickPos = getAndRemoveObject(TileTypes::STAR_BRICK);
    }

    std::shared_ptr<Coin> coin;
    SDL_Rect coinPos = getAndRemoveObject(TileTypes::COIN);
    while (coinPos.x != -1 && coinPos.y != -1) {
        coin = std::make_shared<Coin>(ren, coinPos.x, coinPos.y, true);
        addObject(coin);
        coinPos = getAndRemoveObject(TileTypes::COIN);
    }

    std::shared_ptr<Flag> flag;
    SDL_Rect flagPos = getAndRemoveObject(TileTypes::FLAG);
    while (flagPos.x != -1 && flagPos.y != -1) {
        flag = std::make_shared<Flag>(ren, flagPos.x, flagPos.y);
        addObject(flag);
        flagPos = getAndRemoveObject(TileTypes::FLAG);
    }

    error = 0;
}

TileTypes World::getTileObject(int x, int y) {
    if (x < 0 || x >= 224 || y < 0 || y >= 15) {
        std::cerr << "the requested object is out of map range " << x << ", " << y << std::endl;
        return OUT_OF_MAP;
    }
    return tiles[x][y];
}

/**
 * if returns -1,-1, it means not found.
 * @param type
 * @return
 */
SDL_Rect World::getObject(TileTypes type) {
    SDL_Rect position;
    position.x = -1;
    position.y = -1;
    for (int i = 0; i < 224; i++) {
        for (int j = 0; j < 15; j++) {
            if (getTileObject(i, j) == type) {
                position.x = i;
                position.y = j;
                return position;
            }
        }
    }
    return position;
}

SDL_Rect World::getAndRemoveObject(TileTypes types) {
    SDL_Rect rect = getObject(types);
    if (rect.x != -1 && rect.y != -1) {
        tiles[rect.x][rect.y] = TileTypes::EMPTY;
    }

    return rect;
}

uint32_t World::loadTexture(SDL_Renderer *ren, SDL_Texture *&worldImageTexture, uint32_t &mapWidth, const std::string &imageFile){

    SDL_Surface *worldImageBMP = SDL_LoadBMP(imageFile.c_str());
    if (worldImageBMP == nullptr) {
        std::cout << "SDL_LoadBMP Error: " <<

                  SDL_GetError()

                  <<
                  std::endl;
        return 1;
    }
    worldImageTexture = SDL_CreateTextureFromSurface(ren, worldImageBMP);
    mapWidth = (uint32_t )worldImageBMP->w;
    if (worldImageTexture == nullptr) {
        std::cout << "SDL_CreateTextureFromSurface Error: " <<

                  SDL_GetError()

                  <<
                  std::endl;
        return 1;
    }
    SDL_FreeSurface(worldImageBMP);
    return 0;
}

void World::parseAdvancedFeatures(std::ifstream &mapFile) {
    std::string line;
    enum class ParseStages {PORTALS, UNKNOWN};
    ParseStages currentStage = ParseStages::UNKNOWN;
    while (getline(mapFile, line)) {
        if(line == "[[Portals]]"){
            currentStage = ParseStages::PORTALS;
            continue;
        }
        switch (currentStage) {
            case ParseStages::PORTALS: {
                //sample line for portal
                // coord1,coord2,coord3,coord4,[left,right,down,up],worldName, (Optional)starOverrideX, (Optional)startOverrideY
                bool fail = false;
                std::string tokens[9];
                size_t lastToken = 0;
                size_t newToken = 0;
                for (int i = 0; i < 9; ++i) {
                    newToken = line.find(",", lastToken);
                    tokens[i] = line.substr(lastToken, newToken - lastToken);
                    if(newToken == std::string::npos) {
                        if(i == 6){
                            //this is the case where no start override is going to be set, but start animation is
                            tokens[8] = tokens[6];
                            tokens[6] = "";
                            break;
                        } else if((i == 5 || i == 7 || i == 8)) {
                            break;
                        } else {
                            fail = true;
                        }
                    }
                    lastToken = newToken +1;
                    if (lastToken == std::string::npos && (i == 5 || i == 7 || i == 8)) {
                        fail = true;
                    }
                }
                if(fail) {
                    std::cerr << "Portal parsing failed, check map file" << std::endl;
                    continue;
                }
                Portal newPortal;
                for (int j = 0; j < 4; ++j) {
                    newPortal.coordinates[j] = std::stoi(tokens[j])*TILE_SIZE;
                }
                if(tokens[4] == "left") {newPortal.moveSide = Sides::LEFT;}
                else if(tokens[4] == "right") {newPortal.moveSide = Sides::RIGHT;}
                else if(tokens[4] == "down") {newPortal.moveSide = Sides::DOWN;}
                else if(tokens[4] == "up") {newPortal.moveSide = Sides::UP;}
                else {std::cerr << "Portal enter animation side not found" << std::endl;}
                newPortal.targetWorld = tokens[5];
                if(!tokens[6].empty()) {
                    newPortal.startOverride = true;
                    newPortal.startPosition[0] = std::stoi(tokens[6]);
                    newPortal.startPosition[1] = std::stoi(tokens[7]);
                }
                if(!tokens[6].empty()) {
                    newPortal.startOverride = true;
                    newPortal.startPosition[0] = std::stoi(tokens[6]);
                    newPortal.startPosition[1] = std::stoi(tokens[7]);
                }
                if(tokens[8] == "left") {newPortal.newWorldAnimSide = Sides::LEFT;}
                else if(tokens[8] == "right") {newPortal.newWorldAnimSide = Sides::RIGHT;}
                else if(tokens[8] == "down") {newPortal.newWorldAnimSide = Sides::DOWN;}
                else if(tokens[8] == "up") {newPortal.newWorldAnimSide = Sides::UP;}
                else if(tokens[8] == "none") {newPortal.newWorldAnimSide = Sides::NONE;}
                else {std::cerr << "Portal exit animation side not found" << std::endl;}
                portals.push_back(newPortal);
            }
            break;
            case ParseStages::UNKNOWN: {
                std::cerr << "Read line without parse stage setting, skipping" << std::endl;
            }
            break;
        }
    }
}

bool
World::checkPortal(AABB *position, World::Sides side, PortalInformation** portalInformation) {
    for (size_t i = 0; i < portals.size(); ++i) {
        if(portals[i].moveSide == side){
            if(position->getLeftBorder() >= portals[i].coordinates[0] &&
                    position->getRightBorder() <= portals[i].coordinates[1] &&
                    //position->getUpBorder() > portals[i].coordinates[2] &&
                    position->getDownBorder() <= portals[i].coordinates[3] &&
                    position->getDownBorder() > portals[i].coordinates[3] - 4
                    ) {
                *portalInformation = new PortalInformation();
                (*portalInformation)->worldName = portals[i].targetWorld;
                (*portalInformation)->startOverride = portals[i].startOverride;
                (*portalInformation)->startPosX = portals[i].startPosition[0];
                (*portalInformation)->startPosY = portals[i].startPosition[1];
                (*portalInformation)->startAnimationSide= portals[i].newWorldAnimSide;
                return true;
            }
        }
    }
    return false;
}

bool World::processInput(const InputHandler *input, long time, PortalInformation **portalInformation) {
    World::Sides moveSide;
    if(input->goLeft) {moveSide = World::Sides::LEFT;}
    else if(input->goRight) {moveSide = World::Sides::RIGHT;}
    else if(input->jumpEvent) {moveSide = World::Sides::UP;}
    else if(input->crouchEvent) {moveSide = World::Sides::DOWN;}
    else {moveSide = World::Sides::NONE;}

    if(portalAnimationStartTime == 0) {
        if (moveSide != World::Sides::NONE && checkPortal(mario->getPosition(),
                                                          moveSide, &activatedPortalInformation)) {

            std::cout << "Portal activate" << std::endl;
            portalStartPositionY = mario->getPosition()->getDownBorder();
            portalStartPositionX = mario->getPosition()->getLeftBorder();
            portalAnimationStartTime = time;
            mario->getPosition()->setPhysicsState(AABB::PhysicsState::KINEMATIC);
            portalEnterSide = moveSide;
        } else {
            mario->move(input->goLeft, input->goRight, input->jumpEvent, input->crouch, input->run);
        }
        return false;
    } else {
        if(animatePortal(time) && !portalTriggeredExternally) {
            *portalInformation = activatedPortalInformation;
            return true;
        }
    }
    return false;
}

bool World::animatePortal(long time) const {
    if (time - portalAnimationStartTime < portalAnimationDuration) {
        if(mario->getPosition()->getPhysicsState() != AABB::PhysicsState::KINEMATIC) {
            std::cout << "mario state not set" << std::endl;
        }
        const int MAXIMUM_MOVEMENT = TILE_SIZE * 2;
        const int MOVEMENT_SPEED = 2;
        switch (portalEnterSide) {
            case Sides::DOWN: {
                if (portalStartPositionY - mario->getPosition()->getDownBorder() < MAXIMUM_MOVEMENT) {
                    mario->getPosition()->setUpBorder(mario->getPosition()->getUpBorder() + MOVEMENT_SPEED);
                    mario->getPosition()->setDownBorder(mario->getPosition()->getDownBorder() + MOVEMENT_SPEED);
                } else {
                    std::cout << "Down movement skip " << std::endl;
                    return true;
                }
            }
                break;
            case Sides::LEFT: {
                if (portalStartPositionX - mario->getPosition()->getLeftBorder() < MAXIMUM_MOVEMENT) {
                    mario->move(true, false, false, false, false);
                } else {
                    std::cout << "left movement skip " << std::endl;
                    return true;

                }
            }
                break;
            case Sides::RIGHT: {
                if (mario->getPosition()->getLeftBorder() - portalStartPositionX < MAXIMUM_MOVEMENT) {
                    mario->move(false, true, false, false, false);
                } else {
                    std::cout << "right movement skip " << std::endl;
                    return true;

                }
            }
                break;
            case Sides::UP: {
                if (portalStartPositionY - mario->getPosition()->getDownBorder() < MAXIMUM_MOVEMENT) {
                    mario->getPosition()->setUpBorder(mario->getPosition()->getUpBorder() - MOVEMENT_SPEED);
                    mario->getPosition()->setDownBorder(mario->getPosition()->getDownBorder() - MOVEMENT_SPEED);
                } else {
                    std::cout << "up movement skip " << std::endl;
                    return true;
                }
                break;
                case Sides::NONE: {
                    std::cout << "Portal animation not playing" << std::endl;
                    return true;
                }
            }
        }
    } else {
        std::cout << "Portal animation Done" << std::endl;
        return true;
    }
    return false;
}

void World::triggerPortalAnimation(World::Sides side, long time) {
    std::cout << "Portal tiggered" << std::endl;
    portalStartPositionY = mario->getPosition()->getDownBorder();
    portalStartPositionX = mario->getPosition()->getLeftBorder();
    portalAnimationStartTime = time;
    mario->getPosition()->setPhysicsState(AABB::PhysicsState::KINEMATIC);
    portalEnterSide = side;
    portalTriggeredExternally = true;
}

