//
// Created by engin on 29.10.2017.
//

#ifndef MARIO_MAP_H
#define MARIO_MAP_H


#include <fstream>

#include <SDL_rect.h>

#include "Map.h"


class Map {
public:
    enum TileTypes {EMPTY = 0, GROUND = 1, BRICK = 2, MINION_MUSROOM = 3,
        BRICK_COIN=4, BRICK_MUSHROOM=5, HIDDEN_MUSHROOM=6, PIPE_EMPTY=7, MARIO = 8};
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
                    tiles[i][lineNumber] = (TileTypes)(line.at(i) - 48);
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
        return tiles[x][y];
    }

    SDL_Rect getObject(TileTypes type) {
        SDL_Rect position;
        position.x = 0;
        position.y = 0;
        for (int i = 0; i < 15; i++) {
            for (int j = 0; j < 90; j++) {
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
