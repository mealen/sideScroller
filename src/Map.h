//
// Created by engin on 29.10.2017.
//

#ifndef MARIO_MAP_H
#define MARIO_MAP_H


#include <fstream>
#include <SDL_rect.h>
#include <iostream>

#include "Constants.h"
#include "Map.h"


class Map {
public:
    enum TileTypes {EMPTY = 0, GROUND = 1, BRICK = 2, BRICK_COIN=3, BRICK_MUSHROOM=4, HIDDEN_MUSHROOM=5, PIPE_EMPTY=6,
        PIPE_PORTAL=7, PIPE_ENEMY=8, PIPE_ENEMY_PORTAL=9, MINION_MUSHROOM=17, MINION_TURTLE=18, PLAYER=74};
//                                                                        A                 B           z
private:
    TileTypes tiles[90][15];
public:

    Map(std::string mapLogicFile, int &error) {
        std::ifstream mapFile("./res/levels/" + mapLogicFile);
        std::string line;
        if (mapFile.is_open()) {
            int lineNumber = 0;
            while (getline(mapFile, line)) {
                //std::cout << line << '\n';
                for (int i = 0; i < line.length(); i++) {
                    tiles[i][lineNumber] = (TileTypes)(line.at(i) - '0');//this removes char 0 to make ascii values match tile numbers
                }
                lineNumber++;
            }
            mapFile.close();
        } else {
            error = 1;
            return;
        }
        error = 0;
    }

    TileTypes getTileObject(int x,int y) {
        if(x <0 || x >= 90 || y < 0 ||y>=15) {
            std::cerr << "the requested object is out of map range " << x << ", " << y << std::endl;
            return GROUND;
        }
        return tiles[x][y];
    }

    SDL_Rect getObject(TileTypes type) {
        SDL_Rect position;
        position.x = 0;
        position.y = 0;
        for (int i = 0; i < 90; i++) {
            for (int j = 0; j < 15; j++) {
                if (getTileObject(i,j) == type) {
                    position.x = i;
                    position.y = j;
                    break;
                }
            }
        }
        return position;
    }
};


#endif //MARIO_MAP_H
