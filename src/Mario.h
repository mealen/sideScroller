//
// Created by engin on 29.10.2017.
//

#ifndef MARIO_MARIO_H
#define MARIO_MARIO_H

#include <vector>
#include <map>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL_rect.h>

#include "Utils.h"


class Mario {
public:
    enum TextureNames {
        STAND, MOVE
    };

private:
    std::map<TextureNames, std::vector<SDL_Texture *>> textures;
    SDL_Rect position;
    int screenWidth;

public:

    Mario(SDL_Rect mapPosition, SDL_Renderer *ren, int screenWidth, int &error);

    ~Mario() {
        SDL_DestroyTexture(textures[STAND][0]);
        SDL_DestroyTexture(textures[MOVE][0]);
        SDL_DestroyTexture(textures[MOVE][1]);
        SDL_DestroyTexture(textures[MOVE][2]);
    }

    SDL_Texture *getTexture(TextureNames requiredTexture, long time) {
        switch (requiredTexture) {
            case STAND:
                return textures[STAND][0];
            case MOVE:
                return textures[MOVE][(time / 75) % 3];
        }
    }

    SDL_Rect getPositionRect() const {
        return position;
    }

    SDL_Rect move(bool left, bool right, bool jump, bool crouch) {
        if (left) {
            position.x -= 2;
            if (position.x < 0) {
                position.x = 0;
            }
        }
        if (right) {
            position.x += 2;
            if (position.x + position.w > screenWidth / 2) {
                position.x = screenWidth / 2 - position.w;
            }
        }
    }

};


#endif //MARIO_MARIO_H
