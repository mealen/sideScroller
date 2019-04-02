#include <SDL_rect.h>
#include <string>
#include <iostream>
#include "AnimatedScore.h"

AnimatedScore::AnimatedScore(int x, int y, long startTime, int score, SDL_Renderer* ren) : x(x), y(y), startTime(startTime), score(score), ren(ren) {
    this->remove = false;
    font = TTF_OpenFont("res/fonts/emulogic.ttf", 8);
    SDL_Color textColor;
    textColor.r = 255;
    textColor.g = 255;
    textColor.b = 255;
    textColor.a = 1;

    SDL_Surface *animatedScoreText = TTF_RenderText_Solid(font, std::to_string(score).c_str(),
                                                          textColor);
    animatedScoreTextTexture = SDL_CreateTextureFromSurface(ren, animatedScoreText);
    SDL_FreeSurface(animatedScoreText);
}

void AnimatedScore::render(long time, int x) {
    if (time > startTime + 1000l) {
        remove = true;
        return;
    }

    int diff = (time - startTime) / 20;
    SDL_Rect animateRect;
    animateRect.x = this->x - x;
    animateRect.y = y - diff;
    animateRect.w = 25;
    animateRect.h = 15;

    SDL_RenderCopy(ren, animatedScoreTextTexture, nullptr, &animateRect);
}

bool AnimatedScore::shouldRemove() {
    return remove;
}