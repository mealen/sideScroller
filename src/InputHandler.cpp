//
// Created by engin on 9.04.2019.
//

#include <iostream>
#include "InputHandler.h"

void InputHandler::readInput() {
        jumpEvent = false;
        crouchEvent = false;
        while (SDL_PollEvent(&e)) {
            //If user closes the window
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            //If user presses any key
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_LSHIFT:
                        run = true;
                        break;
                    case SDLK_d:
                    case SDLK_RIGHT:
                        goRight = true;
                        break;
                    case SDLK_a:
                    case SDLK_LEFT:
                        goLeft = true;
                        break;
                    case SDLK_SPACE:
                    case SDLK_UP:
                        if(!jump) {
                            jumpEvent = true;
                        }
                        jump = true;
                        break;
                    case SDLK_s:
                    case SDLK_DOWN:
                        if(!crouch) {
                            crouchEvent = true;
                        }
                        crouch = true;
                        break;
                    case SDLK_p:
                        stop = true;
                        break;
                    case SDLK_r:
                        restart = true;
                        break;

                }
            }
            if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_LSHIFT:
                        run = false;
                        break;
                    case SDLK_d:
                    case SDLK_RIGHT:
                        goRight = false;
                        break;
                    case SDLK_a:
                    case SDLK_LEFT:
                        goLeft = false;
                        break;
                    case SDLK_SPACE:
                    case SDLK_UP:
                        jump = false;
                        break;
                    case SDLK_s:
                    case SDLK_DOWN:
                        crouch = false;
                        break;
                    case SDLK_p:
                        stop = false;
                        break;
                }
            }
            //If user clicks the mouse
            if (e.type == SDL_MOUSEBUTTONDOWN) {
            }
        }
}
