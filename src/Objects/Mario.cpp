//
// Created by engin on 29.10.2017.
//

#include "Mario.h"

const int Mario::MOVE_SPEED = 4;
const int Mario::JUMP_SPEED = 16;

Mario::Mario(SDL_Rect mapPosition, SDL_Renderer *ren, int screenWidth, int &error)
        : screenWidth(screenWidth)
    {
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

void Mario::render(SDL_Renderer *renderer, int x, int y, long time) {
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
