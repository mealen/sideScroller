#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <memory>

#include "Objects/Mario.h"
#include "World.h"
#include "Objects/Brick.h"
#include "Objects/BrickCoin.h"
#include "Context.h"
#include "Objects/Goomba.h"


class InputStates {
public:
    bool quit = false;
    bool goRight = false;
    bool goLeft = false;
    bool jump = false;
    bool jumpEvent = false;
    bool stop = false;
};



void readInput(InputStates &input) {
    input.jumpEvent = false;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        //If user closes the window
        if (e.type == SDL_QUIT) {
            input.quit = true;
        }
        //If user presses any key
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    input.quit = true;
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    input.goRight = true;
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    input.goLeft = true;
                    break;
                case SDLK_SPACE:
                case SDLK_UP:
                    if(!input.jump) {
                        input.jumpEvent = true;
                    }
                    input.jump = true;
                    break;
                case SDLK_p:
                    input.stop = true;
                    break;
            }
        }
        if (e.type == SDL_KEYUP) {
            switch (e.key.keysym.sym) {
                case SDLK_d:
                case SDLK_RIGHT:
                    input.goRight = false;
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    input.goLeft = false;
                    break;
                case SDLK_SPACE:
                case SDLK_UP:
                    input.jump = false;
                    break;
                case SDLK_p:
                    input.stop = false;
                    break;
            }
        }
        //If user clicks the mouse
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            input.quit = true;
        }
    }
}

int init(std::shared_ptr<Context> &context, std::shared_ptr<Map> &map0101, SDL_Renderer *ren) {


    int error;
    map0101 = std::make_shared<Map>("0101_logic.txt", error);
    if (error != 0) {
        std::cerr << "Error initializing Map, Exiting" << std::endl;
        return -1;
    }

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) { return false; }
    Mix_Music *music = NULL;
    music = Mix_LoadMUS("./res/sounds/overworld.wav");
    //If there was a problem loading the music
    if (music == NULL) {
        return -1;
    }

    //Play the music
    if (Mix_PlayMusic(music, -1) == -1) {
        return 1;
    }

    std::shared_ptr<Mario> mario = std::make_shared<Mario>(map0101->getAndRemoveObject(Map::PLAYER), ren, SCREEN_WIDTH,
                                                                           error);
    if (error != 0) {
        std::cerr << "Error initializing Mario, Exiting" << std::endl;
        return -1;
    }

    std::shared_ptr<World> world = std::make_shared<World>(map0101.get(), ren, mario);

    context = std::make_shared<Context>(world, mario);

    return 0;
}

int main(int argc, char *argv[]) {//these parameters has to be here or SDL_main linking issue arises
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Mario", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (win == nullptr) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == nullptr) {
        SDL_DestroyWindow(win);
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    std::string imagePath = Utils::getResourcePath("levels") + "0101_graph.bmp";
    SDL_Surface *bmp = SDL_LoadBMP(imagePath.c_str());
    if (bmp == nullptr) {
        std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);

    if (tex == nullptr) {
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    int mapWidth = bmp->w;
    SDL_FreeSurface(bmp);
    SDL_Rect sourceRect;

    sourceRect.x = 0;
    sourceRect.y = 0;
    sourceRect.h = SCREEN_HEIGHT;
    sourceRect.w = SCREEN_WIDTH;
    InputStates input;
    input.quit = false;
    std::shared_ptr<Map> map;
    std::shared_ptr<Context> context;

    if (TTF_Init() < 0) {
        std::cerr << "Font Init problem" << std::endl;
        return 1;
    }

    if (init(context, map, ren) == -1) {
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        std::cerr << "Init problem" << std::endl;
        SDL_Quit();
        return 1;
    }



    TTF_Font* font = TTF_OpenFont("res/fonts/emulogic.ttf", 16);
    SDL_Color textColor;
    textColor.r = 1;
    textColor.g = 1;
    textColor.b = 1;
    textColor.a = 0;

    SDL_Surface* deadTextSurface = TTF_RenderText_Solid(font, "Game over! You died!", textColor);
    SDL_Texture *deadTextTexture = SDL_CreateTextureFromSurface(ren, deadTextSurface);


    SDL_RendererFlip leftRightFlip = SDL_FLIP_NONE;
    long time;
    long previousTime = 0;
    const AABB* marioPos = context.get()->getPlayer()->getPosition();
    SDL_Rect marioGrapPos;
    marioGrapPos.w = TILE_SIZE;
    marioGrapPos.h = TILE_SIZE;
    marioGrapPos.y = marioPos->getUpBorder();
    marioGrapPos.x = marioPos->getLeftBorder();

    std::shared_ptr<Brick> brick;
    SDL_Rect brickPos = map->getAndRemoveObject(Map::BRICK);
    while (brickPos.x != -1 && brickPos.y != -1) {
        brick = std::make_shared<Brick>(ren, brickPos.x, brickPos.y);
        context.get()->getWorld()->addObject(brick);
        brickPos = map->getAndRemoveObject(Map::BRICK);
    }

    std::shared_ptr<BrickCoin> brickCoin;
    SDL_Rect brickCoinPos = map->getAndRemoveObject(Map::BRICK_COIN);
    while (brickCoinPos.x != -1 && brickCoinPos.y != -1) {
        brickCoin = std::make_shared<BrickCoin>(ren, brickCoinPos.x, brickCoinPos.y);
        context.get()->getWorld()->addObject(brickCoin);
        brickCoinPos = map->getAndRemoveObject(Map::BRICK_COIN);
    }

    std::shared_ptr<Goomba> goomba;
    SDL_Rect goombaPos = map->getAndRemoveObject(Map::GOOMBA);
    while (goombaPos.x != -1 && goombaPos.y != -1) {
        goomba = std::make_shared<Goomba>(ren, goombaPos.x, goombaPos.y);
        context.get()->getWorld()->addObject(goomba);
        goombaPos = map->getAndRemoveObject(Map::GOOMBA);
    }


    while (!input.quit) {
        if (input.stop) {
            input.stop =false;
            SDL_Delay(1000);//for debugging
            continue;
        }
        time = SDL_GetTicks();

        if(time - previousTime > 1000/60.0f) {
            //First clear the renderer
            SDL_RenderClear(ren);
            //Take a quick break after all that hard work
            //SDL_Delay(50);
            if (!context.get()->getPlayer()->isDead()) {
                readInput(input);
                /*SDL_Rect textRect;
                textRect.x = 0;
                textRect.y = 0;
                textRect.w = 600;
                textRect.h = 200;
                while (!input.jump) {
                    SDL_RenderCopy(ren, tex, &sourceRect, NULL);
                    SDL_RenderCopyEx(ren, context.get()->getPlayer()->getTexture(time), 0, &marioGrapPos, 0, 0, leftRightFlip);
                    context.get()->getWorld()->render(ren, sourceRect.x, sourceRect.y, time);
                    SDL_RenderCopy(ren, deadTextTexture, NULL, &textRect);
                    SDL_RenderPresent(ren);
                    readInput(input);
                }
                input.jump = false;
                input.jumpEvent = false;
                init(context, map, ren);*/
            }
            context.get()->getPlayer()->move(input.goLeft, input.goRight, input.jumpEvent, false);
            marioPos = context.get()->getPlayer()->getPosition();
            if (input.goRight) {
                leftRightFlip = SDL_FLIP_NONE;
            } else if (input.goLeft) {
                leftRightFlip = SDL_FLIP_HORIZONTAL;
            }

            marioGrapPos.y = marioPos->getUpBorder();
            int middleOfScreenPixel = (SCREEN_WIDTH) / 2 - TILE_SIZE;
            //determine where the mario should be in screen, and where background should be
            if (marioPos->getMaxRight() - TILE_SIZE <= middleOfScreenPixel) {
                //if mario is not passed middle of the screen
                sourceRect.x = 0;
                marioGrapPos.x = marioPos->getLeftBorder();
            } else {
                //put mario at middle of the screen, and move background to left
                //but first check if mario has been right before
                int leftMovementAmount = 0;
                if (marioPos->getMaxRight() > marioPos->getRightBorder()) {
                    //use maxleft instead of left
                    leftMovementAmount = marioPos->getMaxRight() - marioPos->getRightBorder();

                }
                sourceRect.x = (marioPos->getMaxRight() - TILE_SIZE) - middleOfScreenPixel;
                marioGrapPos.x = middleOfScreenPixel - leftMovementAmount;
                if (sourceRect.x > mapWidth - SCREEN_WIDTH) {
                    //if end of map, let mario move more, and lock background
                    marioGrapPos.x += sourceRect.x - (mapWidth - SCREEN_WIDTH);//re add the difference
                    sourceRect.x = mapWidth - SCREEN_WIDTH;
                }
            }

            previousTime = time;
            context.get()->getWorld()->stepSimulation(time, context);//FIXME parameter should be shared ptr or something
        }

        //Draw the texture
        SDL_RenderCopy(ren, tex, &sourceRect, NULL);


        //draw the mario
        //std::cout << "drawing mario at " << marioGrapPos.x << ", " << marioGrapPos.y << std::endl;
        SDL_RenderCopyEx(ren, context.get()->getPlayer()->getTexture(time), 0, &marioGrapPos, 0, 0, leftRightFlip);

        context.get()->getWorld()->render(ren, sourceRect.x, sourceRect.y, time);

        //Update the screen
        SDL_RenderPresent(ren);


    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}



