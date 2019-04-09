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
#include "InputHandler.h"


void logFrameRate();

int init(std::shared_ptr<Context> &context, SDL_Renderer *ren, const std::string worldName, World::PortalInformation* portalInformation = nullptr) {
    int error;

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        return false;
    }
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

    std::shared_ptr<World> world = std::make_shared<World>(ren);

    if(portalInformation == nullptr) {
        world->load(worldName, error, music);
    } else {
        world->load(portalInformation->worldName, error, music);
    }

    if (error != 0) {
        std::cerr << "Error initializing Map, Exiting" << std::endl;
        return -1;
    }

    std::shared_ptr<Mario> mario = std::make_shared<Mario>(world->getAndRemoveObject(TileTypes::PLAYER), ren, world->getMapWidth(),
                                                                           error);
    if (error != 0) {
        std::cerr << "Error initializing Mario, Exiting" << std::endl;
        return -1;
    }
    if(portalInformation != nullptr) {
        if(context->getPlayer() != nullptr) {
            context->getPlayer()->copyData(*mario);
        }
        if(portalInformation->startOverride) {
            mario->setPosition(portalInformation->startPosX, portalInformation->startPosY);
        }
    }

    std::shared_ptr<HUD> hud = std::make_shared<HUD>(ren, mario, 300);

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

    InputHandler input;
    std::shared_ptr<Context> context;

    if (TTF_Init() < 0) {
        std::cerr << "Font Init problem" << std::endl;
        return 1;
    }

    if (init(context, ren, "0101") == -1) {
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
                input.readInput();
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
                        input.readInput();
                    }
                    input.jump = false;
                    input.jumpEvent = false;
                    init(context, ren, "0101");
                    context->getHUD()->setPrevTime(time);
                }
            }

            if (input.restart) {
                input.restart = false;
                init(context, ren, "0101");
                context->getHUD()->setPrevTime(time);

            }

            previousTime = time;
            World::PortalInformation* portalInformation;
            if(context->getWorld()->processInput(&input, &portalInformation)){
                if(portalInformation == nullptr) {
                    std::cerr << "Portal detected but information is not returned!" << std::endl;
                } else {
                    init(context, ren, "", portalInformation);
                    delete portalInformation;
                    continue;
                }

            }
            context.get()->getWorld()->stepSimulation(time, context);
        }
        //Draw the world
        context.get()->getWorld()->render(ren, time);
        context.get()->getHUD()->render(time, context->getWorld()->getWorldRenderX());

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