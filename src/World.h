//
// Created by engin on 11.11.2017.
//

#ifndef MARIO_WORLD_H
#define MARIO_WORLD_H


#include "Objects/InteractiveObject.h"
#include "Objects/Mario.h"
#include "Constants.h"
#include <memory>
#include <SDL_ttf.h>

class World {
    std::vector<std::shared_ptr<InteractiveObject>> objects;
    SDL_Texture *coinsTextTexture = nullptr;
    TTF_Font *font = nullptr;
    SDL_Color textColor;
    SDL_Renderer *ren = nullptr;
    std::shared_ptr<Mario> mario = nullptr;
    SDL_Rect coinsRect;
    SDL_Rect coinImgPos;
    SDL_Texture *coinTexture = nullptr;
    TileTypes tiles[224][15];
    SDL_Texture *worldImageTexture;
    uint32_t mapWidth;

    SDL_Rect worldRenderRectangle;
public:

    SDL_Renderer *getRen() const {
        return ren;
    }
    void load(std::string worldName, int &error);
    TileTypes getTileObject(int x, int y);
    SDL_Rect getAndRemoveObject(TileTypes types);
    SDL_Rect getObject(TileTypes type);
    void addObject(std::shared_ptr<InteractiveObject> object) {
        objects.push_back(object);
    }

    uint32_t
    loadTexture(SDL_Renderer *ren, SDL_Texture *&worldImageTexture, uint32_t &mapWidth, const std::string &imageFile);

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
    void render(SDL_Renderer *ren, long time) {
        //calculate x and y from mario position

        AABB* marioPos = mario->getPosition();
        int middleOfScreenPixel = (SCREEN_WIDTH) / 2 - TILE_SIZE;

        if (marioPos->getMaxRight() - TILE_SIZE <= middleOfScreenPixel) {
            //if mario is not passed middle of the screen
            worldRenderRectangle.x = 0;
        } else {
            //put mario at middle of the screen, and move background to left
            //but first check if mario has been right before
            worldRenderRectangle.x = (marioPos->getMaxRight() - TILE_SIZE) - middleOfScreenPixel;
            if ((uint32_t) worldRenderRectangle.x > mapWidth - SCREEN_WIDTH) {
                //if end of map, let mario move more, and lock background
                worldRenderRectangle.x = mapWidth - SCREEN_WIDTH;
            }
        }

        SDL_RenderCopy(ren, worldImageTexture, &worldRenderRectangle, NULL);

        for (unsigned int i = 0; i < objects.size(); ++i) {
            objects[i]->render(ren, worldRenderRectangle.x, worldRenderRectangle.y ,time);
        }
        mario->render(ren, worldRenderRectangle.x, worldRenderRectangle.y, time);



        updateCoins();
        renderCoins();
    }

    /**
     *
     * Checks if the box is at collition with given tile coordinates.
     * The coordinates are not graphical, but logical.
     *
     * @param rightSpeed 1 for right, -1 for left, 0 for stopped
     * @param downSpeed 1 for up, -1 for down, 0 for stopped
     * @return true if collides
     */
    TileTypes collide(int rightSpeed, int downSpeed, long time, std::shared_ptr<Context> context,
                           std::shared_ptr<InteractiveObject> interactiveObject);

    void stepSimulation(long time, std::shared_ptr<Context> context);

    void stepSingleObject(long time, const std::shared_ptr<Context> &context,
                          const std::shared_ptr<InteractiveObject> interactiveObject);

    World(SDL_Renderer *ren) : ren(ren) {
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


        worldRenderRectangle.x = 0;
        worldRenderRectangle.y = 0;
        worldRenderRectangle.h = SCREEN_HEIGHT;
        worldRenderRectangle.w = SCREEN_WIDTH;
    }

    ~World(){
        SDL_DestroyTexture(worldImageTexture);
    }

    void setMario(std::shared_ptr<Mario> mario) {
        this->mario = mario;
    }

    uint32_t getMapWidth() const {
        return mapWidth;
    }
};


#endif //MARIO_WORLD_H


