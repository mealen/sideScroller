#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <memory>

#include "Objects/Mario.h"
#include "World.h"
#include "Objects/Brick.h"
#include "Objects/CoinBox.h"
#include "Objects/MushroomBox.h"
#include "Context.h"
#include "Constants.h"
#include "Objects/Goomba.h"
#include "Objects/HiddenCoinBox.h"
#include "Objects/Koopa.h"
#include "Objects/StarBrick.h"


class InputStates {
public:
    bool quit = false;
    bool goRight = false;
    bool goLeft = false;
    bool crouch = false;
    bool crouchEvent = false;
    bool jump = false;
    bool jumpEvent = false;
    bool stop = false;
    bool run = false;
    bool restart = false;
};


void logFrameRate();

void readInput(InputStates &input) {
    input.jumpEvent = false;
    input.crouchEvent = false;
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
                case SDLK_LSHIFT:
                    input.run = true;
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
                case SDLK_s:
                case SDLK_DOWN:
                    if(!input.crouch) {
                        input.crouchEvent = true;
                    }
                    input.crouch = true;
                    break;
                case SDLK_p:
                    input.stop = true;
                    break;
                case SDLK_r:
                    input.restart = true;
                    break;

            }
        }
        if (e.type == SDL_KEYUP) {
            switch (e.key.keysym.sym) {
                case SDLK_LSHIFT:
                    input.run = false;
                    break;
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
                case SDLK_s:
                case SDLK_DOWN:
                    input.crouch = false;
                    break;
                case SDLK_p:
                    input.stop = false;
                    break;
            }
        }
        //If user clicks the mouse
        if (e.type == SDL_MOUSEBUTTONDOWN) {
        }
    }
}

int init(std::shared_ptr<Context> &context, SDL_Renderer *ren) {
    int error;

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        return false;
    }
    Mix_Music *music = NULL;

    std::shared_ptr<World> world = std::make_shared<World>(ren);

    world->load("0101", error);

    if (error != 0) {
        std::cerr << "Error initializing Map, Exiting" << std::endl;
        return -1;
    }
    
    music = Mix_LoadMUS("./res/sounds/overworld.wav");
    //If there was a problem loading the music
    if (music == NULL) {
        return -1;
    }

    //Play the music
    if (Mix_PlayMusic(music, -1) == -1) {
        return 1;
    }

    std::shared_ptr<Mario> mario = std::make_shared<Mario>(world->getAndRemoveObject(TileTypes::PLAYER), ren, world->getMapWidth(),
                                                                           error);
    if (error != 0) {
        std::cerr << "Error initializing Mario, Exiting" << std::endl;
        return -1;
    }

    std::shared_ptr<HUD> hud = std::make_shared<HUD>(ren, mario);

    world->setMario(mario);
    world->addObject(mario);

    context = std::make_shared<Context>(world, mario, hud);
    return 0;
}



int main(int argc __attribute((unused)), char *argv[] __attribute((unused))) {//these parameters has to be here or SDL_main linking issue arises
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

    InputStates input;
    input.quit = false;
    std::shared_ptr<Context> context;

    if (TTF_Init() < 0) {
        std::cerr << "Font Init problem" << std::endl;
        return 1;
    }

    if (init(context, ren) == -1) {
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


    long time;
    long previousTime = 0;

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
            if (!context->getPlayer()->isDead()) {
                readInput(input);
            } else {
                if(time - context->getPlayer()->getDeadTime() > 1500) {
                    SDL_Rect textRect;
                    textRect.x = 0;
                    textRect.y = 0;
                    textRect.w = 600;
                    textRect.h = 200;
                    while (!input.jump && !input.quit) {
                        context.get()->getWorld()->render(ren, time);
                        SDL_RenderCopy(ren, deadTextTexture, NULL, &textRect);
                        SDL_RenderPresent(ren);
                        readInput(input);
                    }
                    input.jump = false;
                    input.jumpEvent = false;
                    init(context, ren);
                }
            }

            if (input.restart) {
                input.restart = false;
                init(context, ren);
            }

            World::Sides moveSide;
            if(input.goLeft) {moveSide = World::Sides::LEFT;}
            else if(input.goRight) {moveSide = World::Sides::RIGHT;}
            else if(input.jumpEvent) {moveSide = World::Sides::UP;}
            else if(input.crouchEvent) {moveSide = World::Sides::DOWN;}
            else {moveSide = World::Sides::NONE;}

            if(moveSide != World::Sides::NONE && context->getWorld()->checkPortal(context->getPlayer()->getPosition(), moveSide)){
                std::cout << "Portal activate" << std::endl;
            }
            context->getPlayer()->move(input.goLeft, input.goRight, input.jumpEvent, input.crouch, input.run);


            previousTime = time;
            context.get()->getWorld()->stepSimulation(time, context);
        }
        //Draw the world
        context.get()->getWorld()->render(ren, time);
        context.get()->getHUD()->render(time);

        //Update the screen
        logFrameRate();
        SDL_RenderPresent(ren);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

void logFrameRate() {
    static uint32_t totalFrames = 0;
    static long lastLogTime = 0;
    totalFrames++;
    long currentTime = SDL_GetTicks();

    if(currentTime > (lastLogTime + 1000)) {
        lastLogTime = currentTime;
        std::cout << totalFrames <<" fps" << std::endl;
        totalFrames = 0;
    }
}