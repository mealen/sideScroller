#ifndef MARIO_HUD_H
#define MARIO_HUD_H

#include <SDL_ttf.h>

#include "Objects/Mario.h"

class HUD {
public:
    HUD(SDL_Renderer* ren, std::shared_ptr<Mario> mario);
    void render(long time);

private:
    SDL_Renderer* ren = nullptr;
    std::shared_ptr<Mario> mario = nullptr;

    SDL_Texture *coinsTextTexture = nullptr;
    TTF_Font *font = nullptr;
    SDL_Color textColor;
    SDL_Rect coinsRect;
    SDL_Rect coinImgPos;
    SDL_Rect marioTextRect;
    SDL_Texture *marioTextTexture = nullptr;
    SDL_Texture *scoreTexture = nullptr;
    SDL_Rect scoreRect;
    SDL_Texture *coinTexture = nullptr;
    void updateScore();
    void updateCoins();
    void renderCoins();


};

#endif //MARIO_HUD_H
