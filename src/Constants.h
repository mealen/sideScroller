//
// Created by engin on 5.11.2017.
//

#ifndef MARIO_CONSTANTS_H
#define MARIO_CONSTANTS_H

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define TILE_SIZE 32


enum TileTypes {EMPTY = 0, GROUND = 1, COIN_TAKEN = 2, BRICK = 3, COIN_BOX=4, MUSHROOM_BOX=5, HIDDEN_MUSHROOM=6, PIPE_EMPTY=7,
    PIPE_PORTAL=8, PIPE_ENEMY=9, FLOWER = 10, PIPE_ENEMY_PORTAL=17, GOOMBA=18, MINION_TURTLE=19, OUT_OF_MAP=20, MUSHROOM=21, HIDDEN_COIN_BOX=22,
//                                                   A                   B               C            D            E                  F
    FIREBALL=73, PLAYER=74, };
//     y             z

enum class CollisionSide { INVALID, DOWN, UP, LEFT, RIGHT};

#endif //MARIO_CONSTANTS_H
