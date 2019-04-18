//
// Created by engin on 29.10.2017.
//

#include <SDL_mixer.h>
#include <map>
#include "Mario.h"
#include "Fireball.h"
#include "../Context.h"



Mario::Mario(SDL_Rect mapPosition, SDL_Renderer *ren, uint32_t mapWidth, int &error) : mapWidth(mapWidth){
        collisionBox = new AABB(
                mapPosition.x * TILE_SIZE,
                mapPosition.x * TILE_SIZE + TILE_SIZE -1,
                mapPosition.y * TILE_SIZE,
                mapPosition.y * TILE_SIZE + TILE_SIZE -1); //-1 because 32 is not part of box. pixels 0 - TILE_SIZE, TILE_SIZE excluded

    collisionBox->setPhysicsState(AABB::PhysicsState::DYNAMIC);

    for (Status status : {SMALL, BIG}) {
        for (Color color : {NORMAL, BLACK, WHITE}) {
            for (TextureNames textureName : {STAND, MOVE, JUMP, DEAD, CROUCH}) {
                if (textureName == DEAD && !(status == SMALL && color == NORMAL)) {continue;}
                if (textureName == CROUCH && status == SMALL) {continue;}
                if (textureName == MOVE) {
                    for (int i = 0; i < 3; i++) {
                        std::string marioImage = Utils::getResourcePath("mario/" +
                                                                        enumToName(status) + "/" +
                                                                        enumToName(color)) +
                                                                        "mario_" + enumToName(textureName) +
                                                                        std::to_string(i) + ".bmp";
                        SDL_Texture* texture = Utils::loadTexture(ren, marioImage);

                        if (texture == nullptr) {
                            std::cerr << "Error loading Mario textures" << std::endl;
                            error = 1;
                            return;
                        }
                        textures[status][color][textureName].push_back(texture);
                    }
                } else {
                    std::string currentTextureName;
                    if(currentState==CROUCH && !isBig()) {
                        currentTextureName = enumToName(TextureNames::STAND);
                    } else {
                        currentTextureName = enumToName(textureName);
                    }
                    std::string marioImage = Utils::getResourcePath("mario/" +
                                                                    enumToName(status) + "/" +
                                                                    enumToName(color)) +
                                                                    "mario_" + currentTextureName +
                                                                    ".bmp";
                    SDL_Texture* texture = Utils::loadTexture(ren, marioImage);
                    if (texture == nullptr) {
                        std::cerr << "Error loading Mario textures" << std::endl;
                        error = 1;
                        return;
                    }
                    textures[status][color][textureName].push_back(texture);
                }

            }
        }
    }

    growSound = Mix_LoadWAV("./res/sounds/mushroomeat.wav");
    jumpSound = Mix_LoadWAV("./res/sounds/jump.wav");
    jumpBigSound = Mix_LoadWAV("./res/sounds/jumpbig.wav");
    error = 0;
}

void Mario::render(SDL_Renderer *renderer, int x __attribute((unused)), int y __attribute((unused)), long time) {
    if (shrinkStarted && ((time - shrinkStartTime) / 100 ) % 2) {
        return;
    }

    AABB *marioPos = getPosition();
    SDL_RendererFlip leftRightFlip;

    SDL_Rect marioGrapPos;
    marioGrapPos.w = TILE_SIZE;
    marioGrapPos.h = TILE_SIZE;
    marioGrapPos.y = marioPos->getUpBorder();
    marioGrapPos.x = marioPos->getLeftBorder();


    if (moveRight) {
        leftRightFlip = SDL_FLIP_NONE;
    } else {
        leftRightFlip = SDL_FLIP_HORIZONTAL;
    }

    marioGrapPos.y = marioPos->getUpBorder();
    if (isBig() && currentState != CROUCH) {
        marioGrapPos.h = TILE_SIZE * 2;
    } else {
        marioGrapPos.h = TILE_SIZE;
    }
    int middleOfScreenPixel = (SCREEN_WIDTH) / 2 - TILE_SIZE;
    //determine where the mario should be in screen, and where background should be
    if (marioPos->getMaxRight() - TILE_SIZE <= middleOfScreenPixel) {
        //if mario is not passed middle of the screen
        marioGrapPos.x = marioPos->getLeftBorder();
    } else {
        // this is the case mario wants to pass middle of the screen
        // if current mario position is at the end of map, it should be able to
        // if not it should be rendered on center, or left if moved left

        if((uint32_t)marioPos->getMaxRight() >= (mapWidth - (SCREEN_WIDTH/2))) {
            //this case mario can move right most end, so reference point is not the middle of screen
            marioGrapPos.x = marioPos->getRightBorder() - TILE_SIZE - (mapWidth - SCREEN_WIDTH);
        } else {
            //put mario at middle of the screen, world will move background to left
            //but first check if mario has been right before
            int leftMovementAmount = 0;
            if (marioPos->getMaxRight() > marioPos->getRightBorder()) {
                //use maxleft instead of left
                leftMovementAmount = marioPos->getMaxRight() - marioPos->getRightBorder();
            }
            marioGrapPos.x = middleOfScreenPixel - leftMovementAmount;
        }
    }

    //draw the mario
    SDL_RenderCopyEx(renderer, getTexture(time), 0, &marioGrapPos, 0, 0, leftRightFlip);
}

bool Mario::isKilled() const {
    return killed;
}

bool Mario::isGrowStarted() const {
    return growStarted;
}

Mario::Status Mario::getStatus() {
    return status;
}

SDL_Texture * Mario::getTexture(long time) const {
    Color textureColor;

    if (getStar()) {
        long timeDivisor = 75;
        if (time - starStartTime > 6200) {
            timeDivisor = 200;
        }
        int rand = (time / timeDivisor) % 3;
        switch (rand) {
            case 0:
                textureColor = NORMAL;
                break;
            case 1:
                textureColor = BLACK;
                break;
            case 2:
                textureColor = WHITE;
                break;
            default:
                textureColor = NORMAL;
                break;
        }


    } else {
        textureColor = color;
    }
    if(collisionBox->isHasJumped()) {
        if (isGrowStarted()) {
            return textures.at(status).at(textureColor).at(STAND).at(0);
        } else {
            return textures.at(status).at(textureColor).at(JUMP).at(0);
        }
    }
    if (isDead()) {
        if(status == SMALL) {
            // if not small, means dead by dropping, don't assign dead texture
            return textures.at(status).at(textureColor).at(DEAD).at(0);
        }
    }
    switch (currentState) {
        case STAND:
            return textures.at(status).at(textureColor).at(currentState).at(0);
        case MOVE:
            return textures.at(status).at(textureColor).at(MOVE).at((time / 75) % 3);
        case JUMP:
            return textures.at(status).at(textureColor).at(currentState).at(0);
        case CROUCH:
            if(isBig()) {
                return textures.at(status).at(textureColor).at(currentState).at(0);
            } else {
                return textures.at(status).at(textureColor).at(STAND).at(0);
            }

        default:
            std::cerr << "Requested Texture type not found" << std::endl;
            exit(-1);
    }
}

AABB* Mario::getPosition() const {
    return collisionBox;
}

TileTypes Mario::getTileType() const {
    return TileTypes::PLAYER;
}

TileTypes Mario::interactWithSide(std::shared_ptr<Context> context __attribute((unused)), std::shared_ptr<InteractiveObject> otherObject,
                                  CollisionSide interactionSide, long time __attribute((unused))) {
    if(jumpSoundChannel != -1 && interactionSide != CollisionSide::DOWN && Mix_Playing(jumpSoundChannel)) {
        Mix_HaltChannel(jumpSoundChannel);
        jumpSoundChannel = -1;
    }
    if (star) {
        otherObject->die(getTileType());
    }
    if (otherObject->getTileType() == TileTypes::GOOMBA && interactionSide != CollisionSide::DOWN) {
        if (isShrinkStarted()) {
            return TileTypes::EMPTY;
        } else {
            if (!star) {
                die(otherObject->getTileType());
            }
        }
    }
    if (otherObject->getTileType() == TileTypes::MUSHROOM) {
        grow();
    }
    return this->getTileType();//no interaction yet
}

bool Mario::waitingForDestroy() {
    return false; //there is no case we are expecting removal
}

void Mario::step(std::shared_ptr<Context> context, long time) {
    if(jumpSoundChannel != -1 && !Mix_Playing(jumpSoundChannel)) {
        jumpSoundChannel = -1;
    }
    if (fireTriggered) {
        int fireballLeftBorder = (this->getPosition()->getLeftBorder()) - (TILE_SIZE / 2);
        Fireball::Direction fireballDirection = Fireball::Direction::LEFT_DOWN;

        if (moveRight) {
            fireballLeftBorder = this->getPosition()->getLeftBorder() + TILE_SIZE;
            fireballDirection = Fireball::Direction::RIGHT_DOWN;
        }

        std::shared_ptr<Fireball> fireball = std::make_shared<Fireball>(context->getWorld()->getRen(),
                                   fireballLeftBorder,
                                   (this->getPosition()->getUpBorder() + (TILE_SIZE)));

        fireball->setDirection(fireballDirection);

        context->getWorld()->addObject(fireball);
        fireTriggered = false;
        fireStartTime = time;
    }

    if (time - fireStartTime > 500) {
        fireStartTime = 0;
    }

    if(!isDead()) {
        lastStepTime = time;
    }
    if (growStarted && growStartTime == 0) {
        Mix_PlayChannel(-1, growSound, 0);
        growStartTime = time;
    }

    if (shrinkStarted && shrinkStartTime == 0) {
        shrinkStartTime = time;
    }

    if (growStarted) {
        getPosition()->setPhysicsState(AABB::STATIC);
        grow();
        if (((time - growStartTime) / 100 ) % 2) {
            shrink();
        } else {
            grow();
        }

    } else if (shrinkStarted) {
        if (((time - growStartTime) / 100 ) % 2) {
            shrink();
        }
    }


    if (shrinkStarted && time - shrinkStartTime > 2000) {
        shrinkStarted = false;
        shrinkStartTime = 0;
        if (!killed) {
            getPosition()->setPhysicsState(AABB::DYNAMIC);

        }
    }

    if (growStarted && time - growStartTime > 1000) {
        growStarted = false;
        growStartTime = 0;
        if (!killed) {
            getPosition()->setPhysicsState(AABB::DYNAMIC);
        }
    }

    if (time - starStartTime > 8000) {
        star = false;
    }
}

void Mario::die(TileTypes type) {
    // you can not kill a dead man
    if (isDead()) {
        return;
    }
    if(type == TileTypes::OUT_OF_MAP) {
        //die without shrink
        InteractiveObject::die(type);
        return;
    }
    if (type == TileTypes::TIME) {
        InteractiveObject::die(type);
        getPosition()->setUpwardSpeed(JUMP_SPEED / 2);
        killed = true;
        getPosition()->setPhysicsState(AABB::NON_INTERACTIVE);
        getPosition()->setHorizontalSpeed(0);
        return;
    }

    if (star) {
        return;
    }
    if (shrinkStarted) {
        return;
    }
    if (isBig() && !shrinkStarted) {
        shrink();
        shrinkStarted = true;
        return;
    }
    if (isDead()) {
        return;
    }
    InteractiveObject::die(type);
    if (type == TileTypes::GOOMBA || type == TileTypes::KOOPA) {
        //getPosition()->setUpBorder(collisionBox->getUpBorder()+TILE_SIZE);
        getPosition()->setUpwardSpeed(JUMP_SPEED / 2);
        //collisionBox->jump(JUMP_SPEED);
        killed = true;
        getPosition()->setPhysicsState(AABB::NON_INTERACTIVE);
        getPosition()->setHorizontalSpeed(0);
    }
}

void Mario::move(bool left, bool right, bool jump, bool crouch, bool run) {
    if (growStarted) {
        return;
    }
    if (isDead()) {
        return;
    }
    if (jump) {
        if(isBig()) {
            jumpSoundChannel = Mix_PlayChannel(-1, jumpBigSound, 0);
        } else {
            jumpSoundChannel = Mix_PlayChannel(-1, jumpSound, 0);
        }
        currentState = JUMP;
        collisionBox->jump(JUMP_SPEED);

    }
    if(!crouch && currentState == CROUCH) {
        currentState = STAND;
        if(isBig()) {
            getPosition()->setUpBorder(getPosition()->getUpBorder() - TILE_SIZE);
        }
    }
    if(crouch) {
        if(currentState != CROUCH) {
            currentState = CROUCH;
            if (isBig()) {
                //make the collision box small again
                getPosition()->setUpBorder(getPosition()->getUpBorder() + TILE_SIZE);
            }
        }
    } else if (left) {
        moveRight = false;
        currentState = MOVE;
        //don't allow going back
        //if not end of map, check with middle of screen
        // if end of map, check with end of screen
        if((uint32_t)collisionBox->getMaxRight() + SCREEN_WIDTH < mapWidth) {
            if (collisionBox->getLeftBorder() + (10 * TILE_SIZE) > collisionBox->getMaxRight()) {
                collisionBox->moveLeft(moveSpeed);
            }
        } else {
            if ((uint32_t)collisionBox->getLeftBorder() > mapWidth - SCREEN_WIDTH) {
                collisionBox->moveLeft(moveSpeed);
            }
        }
    } else if (right) {
        moveRight = true;
        currentState = MOVE;
        collisionBox->moveRight(moveSpeed);
    } else if (!left && !right) {
        currentState = STAND;
    }

    if (canFire() && run && fireStartTime == 0 && !isRunning()) {
        fireTriggered = true;
    }

    if (!fireTriggered) {
        setRunning(run);
    }
}

Mario::~Mario() {
    for (Status status : {SMALL, BIG}) {
        for (Color color : {NORMAL, BLACK, WHITE}) {
            for (TextureNames textureName : {STAND, MOVE, JUMP, DEAD}) {
                for (SDL_Texture* texture : textures[status][color][textureName]) {
                    SDL_DestroyTexture(texture);
                }
            }
        }
    }

    delete collisionBox;
}

bool Mario::grow() {
    if (!isBig()) {
        growStarted = true;
        currentState = STAND;
        this->status = BIG;
        getPosition()->setUpBorder(getPosition()->getUpBorder() - TILE_SIZE);
        return true;
    } else {
        return isBig();
    }
}

bool Mario::shrink() {
    if (isBig()) {
        if (canFire()) {
            setFire(false);
        }
        this->status = Status::SMALL;
        currentState = STAND;
        this->color = Color::NORMAL;
        getPosition()->setUpBorder(getPosition()->getUpBorder() + TILE_SIZE);
        return true;
    } else {
        return false;
    }
}

bool Mario::isBig() const {
    return status == Status::BIG || canFire();
}

long Mario::getDeadTime() const {
    return lastStepTime;

}

int Mario::getCoins() const {
    return coins;
}

int Mario::getScore() const {
    return score;
}

int Mario::increaseCoin(int amount) {
    coins += amount;
    score += amount * 500;
    return coins;
}

int Mario::increaseScore(int amount) {
    score += amount;
    return amount;
}

bool Mario::isShrinkStarted() const {
    return shrinkStarted;
}

bool Mario::isRunning() const {
    return this->moveSpeed == 6;
}


void Mario::setRunning(bool run) {
    if (run) {
        this->moveSpeed = 6;
    } else {
        this->moveSpeed = 4;
    }
}

bool Mario::canFire() const {
    return fire;
}

void Mario::setFire(bool fire) {
    if (fire) {
        if (isBig()) {
            this->fire = fire;
            color = Color::WHITE;
        } else {
            grow();
        }
    } else {
        this->fire = fire;
    }

}

void Mario::setStar(long starTime) {
    this->star = true;
    this->starStartTime = starTime;

}

bool Mario::getStar() const {
    return star;
}

std::string Mario::enumToName(Mario::Status status) {
    switch(status) {
        case Mario::Status::SMALL:
            return "small";
        case Mario::Status::BIG:
            return "big";
        default:
            return nullptr;
    }
}

std::string Mario::enumToName(Mario::Color status) {
    switch(status) {
        case Mario::Color::NORMAL:
            return "normal";
        case Mario::Color::BLACK:
            return "black";
        case Mario::Color::WHITE:
            return "white";
        default:
            return nullptr;
    }
}

std::string Mario::enumToName(Mario::TextureNames status) {
    switch(status) {
        case Mario::TextureNames::STAND:
            return "stand";
        case Mario::TextureNames::MOVE:
            return "move";
        case Mario::TextureNames::JUMP:
            return "jump";
        case Mario::TextureNames::DEAD:
            return "death";
        case Mario::TextureNames::CROUCH:
            return "crouch";
        default:
            return nullptr;
    }
}

void Mario::setPosition(int xPosition, int yPosition) {
    /**
     *
                mapPosition.x * TILE_SIZE,
                mapPosition.x * TILE_SIZE + TILE_SIZE -1,
                mapPosition.y * TILE_SIZE,
                mapPosition.y * TILE_SIZE + TILE_SIZE -1); //-1 because 32 is not part of box. pixels 0 - TILE_SIZE, TILE_SIZE excluded
     */
    this->getPosition()->setLeftBorder(xPosition * TILE_SIZE);
    this->getPosition()->setRightBorder(xPosition * TILE_SIZE + TILE_SIZE -1);
    this->getPosition()->setUpBorder(yPosition * TILE_SIZE);
    this->getPosition()->setDownBorder(yPosition * TILE_SIZE + TILE_SIZE -1);
    if(isBig()) {
        getPosition()->setUpBorder(getPosition()->getUpBorder() - TILE_SIZE);
    }
}
