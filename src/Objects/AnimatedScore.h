#ifndef MARIO_ANIMATEDSCORE_H
#define MARIO_ANIMATEDSCORE_H

#include <SDL_render.h>
#include <SDL_ttf.h>

class AnimatedScore {
public:
    AnimatedScore(int x, int y, long startTime, int score, SDL_Renderer* ren);
    void render(long time, int x);
    bool shouldRemove();
private:
    int x;
    int y;
    long startTime;
    int score;
    bool remove;
    SDL_Renderer* ren;
    TTF_Font *font = nullptr;
    SDL_Texture* animatedScoreTextTexture;
};

#endif //MARIO_ANIMATEDSCORE_H
