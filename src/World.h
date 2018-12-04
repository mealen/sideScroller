//
// Created by engin on 11.11.2017.
//

#ifndef MARIO_WORLD_H
#define MARIO_WORLD_H


#include "Objects/InteractiveObject.h"
#include "Objects/Mario.h"
#include <memory>
#include <SDL_ttf.h>

class World {
    std::vector<std::shared_ptr<InteractiveObject>> objects;
    Map* map;
    SDL_Texture *coinsTextTexture = nullptr;
    TTF_Font *font = nullptr;
    SDL_Color textColor;
    SDL_Renderer *ren = nullptr;
    std::shared_ptr<Mario> mario = nullptr;
    SDL_Rect coinsRect;
    SDL_Rect coinImgPos;
    SDL_Texture *coinTexture = nullptr;

public:
    SDL_Renderer *getRen() const {
        return ren;
    }

public:
    void addObject(std::shared_ptr<InteractiveObject> object) {
        objects.push_back(object);
    }

    void updateCoins() {
        SDL_DestroyTexture(coinsTextTexture);
        SDL_Surface *coinsTextSurface = TTF_RenderText_Solid(font, ("*" + std::to_string(mario->getCoins())).c_str(),
                                                             textColor);
        coinsTextTexture = SDL_CreateTextureFromSurface(ren, coinsTextSurface);
        SDL_FreeSurface(coinsTextSurface);
    }

    void renderCoins() {
        SDL_RenderCopy(ren, coinTexture, nullptr, &coinImgPos);
        SDL_RenderCopy(ren, coinsTextTexture, NULL, &coinsRect);
    }

    /**
     *
     * @param ren  renderer
     * @param x -> current world x
     * @param y -> current world y
     * @param time -> get ticks
     */
    void render(SDL_Renderer *ren, int x, int y, long time) {
        for (unsigned int i = 0; i < objects.size(); ++i) {
            objects[i]->render(ren,x,y,time);
        }
        updateCoins();
        renderCoins();
    }

    /**
     *
     * Checks if the box is at collition with given tile coordinates.
     * The coordinates are not graphical, but logical.
     *
     * @param map
     * @param rightSpeed 1 for right, -1 for left, 0 for stopped
     * @param downSpeed 1 for up, -1 for down, 0 for stopped
     * @return true if collides
     */
    Map::TileTypes collide(int rightSpeed, int downSpeed, long time, std::shared_ptr<Context> context,
                           std::shared_ptr<InteractiveObject> interactiveObject);

    void stepSimulation(long time, std::shared_ptr<Context> context) {
        for (size_t i = 0; i < this->objects.size(); ++i) {
            this->objects[i]->step(time);
        }
        stepSingleObject(time, context, this->mario);
        for (size_t i = 0; i < this->objects.size(); ++i) {
            std::shared_ptr<InteractiveObject> interactiveObject = this->objects[i];
            stepSingleObject(time, context, interactiveObject);
        }

        for(unsigned int i = 0; i < objects.size(); i++) {
            if(objects[i]->waitingForDestroy()) {
                objects.erase(objects.begin() + i);
            }
        }
    }

    void stepSingleObject(long time, const std::shared_ptr<Context> &context,
                          const std::shared_ptr<InteractiveObject> interactiveObject) {
        AABB *aabb = interactiveObject->getPosition();

        if(aabb->getPhysicsState() == AABB::PhysicsState::STATIC) {
            return;
        }

        int horizontalSpeed = aabb->getHorizontalSpeed();

        Map::TileTypes tile = collide(horizontalSpeed, 0, time, context, interactiveObject);

        if (tile == Map::EMPTY || aabb->getPhysicsState() != AABB::DYNAMIC) {
            aabb->setLeftBorder(aabb->getLeftBorder() + horizontalSpeed);
            aabb->setRightBorder(aabb->getRightBorder() + horizontalSpeed);
        }

        aabb->setHorizontalSpeed(0);

        if (aabb->isHasJumpTriggered()) {
            aabb->setHasJumpTriggered(false);
            tile = collide(0, 1, time, context, interactiveObject);

            if (tile != Map::EMPTY && aabb->getPhysicsState() == AABB::DYNAMIC) {
                aabb->setUpwardSpeed(aabb->getUpwardRequest());
            }
            aabb->setUpwardRequest(0);
        }
        int upwardSpeed = aabb->getUpwardSpeed();
        tile = collide(0, -1 * upwardSpeed, time, context, interactiveObject);
        //check if moving with upward speed is possible
        if (tile == Map::OUT_OF_MAP || aabb->getPhysicsState() != AABB::DYNAMIC) {
            if (aabb->getUpwardSpeed() < 0) {
                // mario dies
                interactiveObject->die(tile);
                std::cout << "Mario dies\n";
            }
        }
        if (tile != Map::EMPTY) {//if not possible, match the tile, and then stop
            aabb->setUpBorder(aabb->getUpBorder() - upwardSpeed);
            if (aabb->getUpwardSpeed() > 0) {
                aabb->setUpBorder(((aabb->getUpBorder() + TILE_SIZE) / TILE_SIZE) * TILE_SIZE);
            } else {
                aabb->setUpBorder((aabb->getUpBorder() / TILE_SIZE) * TILE_SIZE);
            }
            aabb->setDownBorder(aabb->getUpBorder() + TILE_SIZE - 1);
            aabb->setUpwardSpeed(0);
            aabb->setHasJumped(false);
        } else { //if possible update
            aabb->setUpBorder(aabb->getUpBorder() - aabb->getUpwardSpeed());
            aabb->setDownBorder(aabb->getDownBorder() - aabb->getUpwardSpeed());
            aabb->setUpwardSpeed(aabb->getUpwardSpeed() - 1);
        }
    }

    World(Map *map, SDL_Renderer *ren, std::shared_ptr<Mario> mario) : map(map), ren(ren), mario(mario) {
        font = TTF_OpenFont("res/fonts/emulogic.ttf", 8);
        textColor.r = 255;
        textColor.g = 255;
        textColor.b = 255;
        textColor.a = 1;

        coinsRect.x = 300;
        coinsRect.y = 10;
        coinsRect.w = 35;
        coinsRect.h = 15;

        coinImgPos.x = 290;
        coinImgPos.y = 10;
        coinImgPos.w = 10;
        coinImgPos.h = 16;

        coinTexture = Utils::loadTexture(ren, Utils::getResourcePath() + "coin_text_icon.bmp");
        updateCoins();

    }

};


#endif //MARIO_WORLD_H


