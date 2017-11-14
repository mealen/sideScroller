#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <map>

#include "Constants.h"
#include "Utils.h"
#include "Objects/Mario.h"
#include "Map.h"
#include "World.h"
#include "Objects/Brick.h"


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
                    input.goRight = true;
                    break;
                case SDLK_a:
                    input.goLeft = true;
                    break;
                case SDLK_SPACE:
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
                    input.goRight = false;
                    break;
                case SDLK_a:
                    input.goLeft = false;
                    break;
                case SDLK_SPACE:
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

    int error;
    Map map0101("0101_logic.txt", error);
    if (error != 0) {
        std::cerr << "Error initializing Map, Exiting" << std::endl;
        return -1;
    }

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) { return false; }
    Mix_Music *music = NULL;
    music = Mix_LoadMUS("./res/sounds/overworld.wav");
    //If there was a problem loading the music
    if (music == NULL) {
        return false;
    }

    //Play the music
    if (Mix_PlayMusic(music, -1) == -1) {
        return 1;
    }

    InputStates input;
    input.quit = false;
    Mario mario(map0101.getAndRemoveObject(Map::PLAYER), ren, SCREEN_WIDTH,
                error);
    if (error != 0) {
        std::cerr << "Error initializing Mario, Exiting" << std::endl;
        return -1;
    }

    World world(&map0101);


    SDL_RendererFlip leftRightFlip = SDL_FLIP_NONE;
    long time;
    long previousTime = 0;
    const AABB* marioPos = mario.getPosition();
    SDL_Rect marioGrapPos;
    marioGrapPos.w = TILE_SIZE;
    marioGrapPos.h = TILE_SIZE;
    marioGrapPos.y = marioPos->getUpBorder();
    marioGrapPos.x = marioPos->getLeftBorder();

    Brick* brick;
    SDL_Rect brickPos = map0101.getAndRemoveObject(Map::BRICK);
    while (brickPos.x != -1 && brickPos.y != -1) {
        brick = new Brick(ren, brickPos.x, brickPos.y);
        world.addObject(brick);
        brickPos = map0101.getAndRemoveObject(Map::BRICK);

    }

    while (!input.quit) {
        if (input.stop) {
            input.stop =false;
            SDL_Delay(1000);//for debugging
            continue;
        }
        time = SDL_GetTicks();

        if(time - previousTime > 1000/60.0f) {
            readInput(input);
            //First clear the renderer
            SDL_RenderClear(ren);
            //Take a quick break after all that hard work
            //SDL_Delay(50);
            mario.move(input.goLeft, input.goRight, input.jumpEvent, false);
            marioPos = mario.getPosition();
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
            world.step(&mario, time);
        }

        //Draw the texture
        SDL_RenderCopy(ren, tex, &sourceRect, NULL);


        //draw the mario
        //std::cout << "drawing mario at " << marioGrapPos.x << ", " << marioGrapPos.y << std::endl;
        SDL_RenderCopyEx(ren, mario.getTexture(time), 0, &marioGrapPos, 0, 0, leftRightFlip);

        world.render(ren, sourceRect.x, sourceRect.y, time);

        //Update the screen
        SDL_RenderPresent(ren);


    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}



