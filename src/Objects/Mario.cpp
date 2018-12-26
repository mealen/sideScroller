//
// Created by engin on 29.10.2017.
//

#include <SDL_mixer.h>
#include "Mario.h"

const int Mario::MOVE_SPEED = 4;
const int Mario::JUMP_SPEED = 16;

Mario::Mario(SDL_Rect mapPosition, SDL_Renderer *ren, int &error) {
        collisionBox = new AABB(
                mapPosition.x * TILE_SIZE,
                mapPosition.x * TILE_SIZE + TILE_SIZE -1,
                mapPosition.y * TILE_SIZE,
                mapPosition.y * TILE_SIZE + TILE_SIZE -1); //-1 because 32 is not part of box. pixels 0 - TILE_SIZE, TILE_SIZE excluded

    collisionBox->setPhysicsState(AABB::PhysicsState::DYNAMIC);

    std::string marioImage = Utils::getResourcePath("mario") + "mario.bmp";
    textures[SMALL][STAND].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_move0.bmp";
    textures[SMALL][MOVE].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_move1.bmp";
    textures[SMALL][MOVE].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_move2.bmp";
    textures[SMALL][MOVE].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_jump.bmp";
    textures[SMALL][JUMP].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario") + "mario_death.bmp";
    textures[SMALL][DEAD].push_back(Utils::loadTexture(ren, marioImage));


    marioImage = Utils::getResourcePath("mario/big") + "mario.bmp";
    textures[BIG][STAND].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario/big") + "mario_move0.bmp";
    textures[BIG][MOVE].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario/big") + "mario_move1.bmp";
    textures[BIG][MOVE].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario/big") + "mario_move2.bmp";
    textures[BIG][MOVE].push_back(Utils::loadTexture(ren, marioImage));

    marioImage = Utils::getResourcePath("mario/big") + "mario_jump.bmp";
    textures[BIG][JUMP].push_back(Utils::loadTexture(ren, marioImage));

    growSound = Mix_LoadWAV("./res/sounds/mushroomeat.wav");

        if (textures[SMALL][STAND][0] == nullptr ||
        textures[SMALL][MOVE][0] == nullptr ||
        textures[SMALL][MOVE][1] == nullptr ||
        textures[SMALL][MOVE][2] == nullptr ||
        textures[SMALL][JUMP][0] == nullptr ||
        textures[BIG][STAND][0] == nullptr ||
        textures[BIG][MOVE][0] == nullptr ||
        textures[BIG][MOVE][1] == nullptr ||
        textures[BIG][MOVE][2] == nullptr ||
        textures[BIG][JUMP][0] == nullptr ) {
        std::cerr << "Error loading Mario textures" << std::endl;
        error = 1;
        return;
    }

    error = 0;
}

void Mario::render(SDL_Renderer *renderer, int x __attribute((unused)), int y __attribute((unused)), long time) {
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
    if (getBig()) {
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
        //put mario at middle of the screen, and move background to left
        //but first check if mario has been right before
        int leftMovementAmount = 0;
        if (marioPos->getMaxRight() > marioPos->getRightBorder()) {
            //use maxleft instead of left
            leftMovementAmount = marioPos->getMaxRight() - marioPos->getRightBorder();

        }
        marioGrapPos.x = middleOfScreenPixel - leftMovementAmount;

    }

    //FIXME: we should check if we are at the end of the level and move only mario


    //draw the mario
    SDL_RenderCopyEx(renderer, getTexture(time), 0, &marioGrapPos, 0, 0, leftRightFlip);
}

bool Mario::isKilled() const {
    return killed;
}

bool Mario::isGrowStarted() const {
    return growStarted;
}

SDL_Texture * Mario::getTexture(long time) const {
    Status curStatus;
    if (isBig) {
        curStatus = BIG;
    } else {
        curStatus = SMALL;
    }
    if(collisionBox->isHasJumped()) {
        return textures.at(curStatus).at(JUMP).at(0);
    }
    if (isDead()) {
        return textures.at(curStatus).at(DEAD).at(0);
    }
    switch (currentState) {
        case STAND:
            return textures.at(curStatus).at(STAND).at(0);
        case MOVE:
            return textures.at(curStatus).at(MOVE).at((time / 75) % 3);
        default:
            std::cerr << "Requested Texture type not found" << std::endl;
            exit(-1);
    }
}

AABB* Mario::getPosition() const {
    return collisionBox;
}

Map::TileTypes Mario::getTileType() const {
    return Map::PLAYER;
}

Map::TileTypes Mario::interactWithSide(std::shared_ptr<Context> context __attribute((unused)), std::shared_ptr<InteractiveObject> otherObject,
                                int interactionSide, long time __attribute((unused))) {
    if (otherObject->getTileType() == Map::TileTypes::GOOMBA && interactionSide != 1) {
        die(otherObject->getTileType());
    }
    if (otherObject->getTileType() == Map::TileTypes::MUSHROOM) {
        grow();
    }
    return this->getTileType();//no interaction yet
}

bool Mario::waitingForDestroy() {
    return false; //there is no case we are expecting removal
}

void Mario::step(long time) {
    if(!isDead()) {
        lastStepTime = time;
    }
    if (growStarted && growStartTime == 0) {
        Mix_PlayChannel(-1, growSound, 0);
        growStartTime = time;
    }

    if (growStarted) {
        getPosition()->setPhysicsState(AABB::STATIC);
        if (((time - growStartTime) / 100 ) % 2) {
            shrink();
        } else {
            grow();
        }
    } else if (!killed) {
        getPosition()->setPhysicsState(AABB::DYNAMIC);

}

    if (time - growStartTime > 1000) {
        growStarted = false;
        growStartTime = 0;
    }
};

void Mario::die(Map::TileTypes type) {
    if (getBig()) {
        shrink();
        return;
    }
    if (isDead()) {
        return;
    }
    InteractiveObject::die(type);
    if (type == Map::TileTypes::GOOMBA) {
        //getPosition()->setUpBorder(collisionBox->getUpBorder()+TILE_SIZE);
        getPosition()->setUpwardSpeed(Mario::JUMP_SPEED / 2);
        //collisionBox->jump(JUMP_SPEED);
        killed = true;
        getPosition()->setPhysicsState(AABB::KINEMATIC);
        getPosition()->setHorizontalSpeed(0);
    }
}

void Mario::move(bool left, bool right, bool jump, bool crouch __attribute((unused))) {
    if (growStarted) {
        return;
    }
    if(isDead()) {
        return;
    }
    if(jump) {
        collisionBox->jump(JUMP_SPEED);
    }
    if (left) {
        moveRight = false;
        currentState = MOVE;
        if (collisionBox->getLeftBorder() + (320) > collisionBox->getMaxRight()) {
            collisionBox->moveLeft(MOVE_SPEED);
        }
    }
    if (right) {
        moveRight = true;
        currentState = MOVE;
        collisionBox->moveRight(MOVE_SPEED);
    }
    if(!left && !right) {
        currentState = STAND;
    }
}

Mario::~Mario() {
    SDL_DestroyTexture(textures[SMALL][STAND][0]);
    SDL_DestroyTexture(textures[SMALL][MOVE][0]);
    SDL_DestroyTexture(textures[SMALL][MOVE][1]);
    SDL_DestroyTexture(textures[SMALL][MOVE][2]);
    SDL_DestroyTexture(textures[SMALL][JUMP][0]);
    SDL_DestroyTexture(textures[BIG][STAND][0]);
    SDL_DestroyTexture(textures[BIG][MOVE][0]);
    SDL_DestroyTexture(textures[BIG][MOVE][1]);
    SDL_DestroyTexture(textures[BIG][MOVE][2]);
    SDL_DestroyTexture(textures[BIG][JUMP][0]);
    delete collisionBox;
}

bool Mario::grow() {
    if (!isBig) {
        growStarted = true;
        isBig = true;
        getPosition()->setUpBorder(getPosition()->getUpBorder() - TILE_SIZE);
        return true;
    } else {
        return false;
    }
}

bool Mario::shrink() {
    if (isBig) {
        isBig = false;
        getPosition()->setUpBorder(getPosition()->getUpBorder() + TILE_SIZE);
        return true;
    } else {
        return false;
    }
}

bool Mario::getBig() {
    return isBig;
}

long Mario::getDeadTime() const {
    return lastStepTime;

}

int Mario::getCoins() const {
    return coins;
}


int Mario::increaseCoin(int amount) {
    coins += amount;
    return coins;
}