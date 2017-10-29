#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <map>
#include "Utils.h"
#include "Mario.h"
#include "Map.h"


class InputStates {
public:
    bool quit = false;
    bool goRight = false;
    bool goLeft = false;
};



void readInput(InputStates &input) {
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
            }
        }
        //If user clicks the mouse
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            input.quit = true;
        }
    }
}

int main(int argc, char *argv[]) {//these parameters has to be here or SDL_main linking issue arises
    std::cout << "Hello, World!" << std::endl;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
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
    sourceRect.h = 480;
    sourceRect.w = 640;

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
    Mario mario(map0101.getObject(Map::MARIO), ren, 640,
                map0101,
                error);
    if (error != 0) {
        std::cerr << "Error initializing Mario, Exiting" << std::endl;
        return -1;
    }


    SDL_RendererFlip leftRightFlip = SDL_FLIP_NONE;
    long time;
    AABB marioPos = mario.getPositionRect();
    SDL_Rect marioGrapPos;
    marioGrapPos.w = 32;
    marioGrapPos.h = 32;
    marioGrapPos.y = marioPos.y1;
    marioGrapPos.x = marioPos.x1;
    Mario::TextureNames marioTextureName = Mario::STAND;
    while (!input.quit) {
        time = SDL_GetTicks();
        readInput(input);
        //First clear the renderer
        SDL_RenderClear(ren);
        //Take a quick break after all that hard work
        //SDL_Delay(50);

        if (input.goRight) {
            mario.move(false, true, false, false);
            marioPos = mario.getPositionRect();
            leftRightFlip = SDL_FLIP_NONE;
            marioTextureName = mario.MOVE;
            if (marioPos.x1 >= (640 - 64) / 2) {
                //if mario is beyond half of the screen
                if(sourceRect.x < marioPos.x1 - (640 - 64) / 2) {
                    sourceRect.x = marioPos.x1 - (640 - 64) / 2;
                    marioGrapPos.x = (640 - 64) / 2;
                } else {
                    marioGrapPos.x = marioPos.x1 - sourceRect.x;
                }
                if (sourceRect.x > mapWidth - 640) {
                    sourceRect.x = mapWidth - 640;
                }
            } else {
                marioGrapPos.y = marioPos.y1;
                marioGrapPos.x = marioPos.x1;
            }
        } else if (input.goLeft) {
            mario.move(true, false, false, false);
            marioPos = mario.getPositionRect();
            leftRightFlip = SDL_FLIP_HORIZONTAL;
            marioTextureName = mario.MOVE;
            //don't move the screen, move the mario
            //sourceRect.x; // last position of the screens left most point
            marioGrapPos.x = marioPos.x1 - sourceRect.x;
        } else {
            marioTextureName = mario.STAND;
        }

        //Draw the texture
        SDL_RenderCopy(ren, tex, &sourceRect, NULL);

        //draw the mario
        std::cout << "drawing mario at " << marioGrapPos.x << ", " << marioGrapPos.y << std::endl;
        SDL_RenderCopyEx(ren, mario.getTexture(marioTextureName, time), 0, &marioGrapPos, 0, 0, leftRightFlip);
        //Update the screen
        SDL_RenderPresent(ren);


    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    //return 0;
}



