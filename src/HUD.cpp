#include <memory>
#include <SDL_surface.h>
#include <SDL_render.h>
#include <SDL2/SDL.h>
#include <sstream>
#include <iomanip>
#include "HUD.h"

HUD::HUD(SDL_Renderer* ren, std::shared_ptr<Mario> mario) : ren(ren), mario(mario) {
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

    marioTextRect.x = 25;
    marioTextRect.y = 10;
    marioTextRect.w = 80;
    marioTextRect.h = 16;

    scoreRect.x = 25;
    scoreRect.y = 25;
    scoreRect.w = 100;
    scoreRect.h = 16;

    SDL_Surface *marioTextSurface = TTF_RenderText_Solid(font, "MARIO",
                                                         textColor);
    marioTextTexture = SDL_CreateTextureFromSurface(ren, marioTextSurface);
    SDL_FreeSurface(marioTextSurface);
    coinTexture = Utils::loadTexture(ren, Utils::getResourcePath() + "coin_text_icon.bmp");
}

void HUD::updateCoins() {
    SDL_DestroyTexture(coinsTextTexture);
    SDL_Surface *coinsTextSurface = TTF_RenderText_Solid(font, ("*" + std::to_string(mario->getCoins())).c_str(),
                                                         textColor);
    coinsTextTexture = SDL_CreateTextureFromSurface(ren, coinsTextSurface);
    SDL_FreeSurface(coinsTextSurface);
}

void HUD::renderCoins() {
    SDL_RenderCopy(ren, coinTexture, nullptr, &coinImgPos);
    SDL_RenderCopy(ren, coinsTextTexture, nullptr, &coinsRect);
}

void HUD::render(long time, int x) {
    std::vector<AnimatedScore *>::iterator it;
    for (it = animatedScores.begin(); it != animatedScores.end();) {
        if ((*it)->shouldRemove()) {
            it = animatedScores.erase(it);
        } else {
            (*it)->render(time, x);
            it++;
        }
    }

    updateCoins();
    updateScore();
    SDL_RenderCopy(ren, marioTextTexture, nullptr, &marioTextRect);
    SDL_RenderCopy(ren, scoreTexture, nullptr, &scoreRect);
    renderCoins();
}


void HUD::updateScore() {
    std::ostringstream score;
    score << std::setw(6) << std::setfill('0') << mario->getScore();
    SDL_DestroyTexture(scoreTexture);
    SDL_Surface *scoreSurface = TTF_RenderText_Solid(font, score.str().c_str(),
                                                     textColor);
    scoreTexture = SDL_CreateTextureFromSurface(ren, scoreSurface);
    SDL_FreeSurface(scoreSurface);
}

void HUD::animateScore(int amount, int x, int y, long time) {
    if (scoreComboStartTime == -1) {
        scoreComboStartTime = time;
    }

    if (time - scoreComboStartTime < 1000) {
        comboScoreAmount += 1;
    } else {
        comboScoreAmount = 1;
        scoreComboStartTime = time;
    }

    int score = amount * comboScoreAmount;
    AnimatedScore* as = new AnimatedScore(x, y, time, score, ren);
    mario->increaseScore(score);
    animatedScores.push_back(as);
}