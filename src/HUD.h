#ifndef MARIO_HUD_H
#define MARIO_HUD_H

#include <SDL_ttf.h>

#include "Objects/Mario.h"
#include "Objects/AnimatedScore.h"

class HUD {
public:
    HUD(SDL_Renderer* ren, std::shared_ptr<Mario> mario, int time);
    void render(long time, int x);
    void animateScore(int amount, int x, int y, long time);
    void setTime(int time);
    int getTime() const;
    void setPrevTime(long time);

private:
    SDL_Renderer* ren = nullptr;
    std::shared_ptr<Mario> mario = nullptr;

    std::vector<AnimatedScore *> animatedScores;
    SDL_Texture *coinsTextTexture = nullptr;
    SDL_Texture *timeTextTexture = nullptr;
    SDL_Texture *timeTexture = nullptr;
    TTF_Font *font = nullptr;
    SDL_Color textColor;
    SDL_Rect coinsRect;
    SDL_Rect coinImgPos;
    SDL_Rect marioTextRect;
    SDL_Rect timeTextPos;
    SDL_Rect timePos;
    SDL_Texture *marioTextTexture = nullptr;
    SDL_Texture *scoreTexture = nullptr;
    SDL_Rect scoreRect;
    std::vector<SDL_Texture *> coinTextures;
    void updateScore();
    void updateCoins();
    void updateTime();
    void renderCoins(long time);
    long scoreComboStartTime = -1;
    int comboScoreAmount = 0;
    int time = 0;
    long prevTime = 0;
    SDL_Texture * getCoinTexture(long time) const;


    };

#endif //MARIO_HUD_H
