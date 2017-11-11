//
// Created by engin on 29.10.2017.
//

#ifndef MARIO_MAP_H
#define MARIO_MAP_H


#include <fstream>
#include <SDL_rect.h>
#include <iostream>
#include <vector>

#include "Constants.h"
#include "Map.h"


class Map {
public:
    enum TileTypes {EMPTY = 0, GROUND = 1, BRICK = 2, BRICK_COIN=3, BRICK_MUSHROOM=4, HIDDEN_MUSHROOM=5, PIPE_EMPTY=6,
        PIPE_PORTAL=7, PIPE_ENEMY=8, PIPE_ENEMY_PORTAL=9, MINION_MUSHROOM=17, MINION_TURTLE=18, PLAYER=74};
//                                                                        A                 B           z
private:
    TileTypes tiles[224][15];
public:

    Map(std::string mapLogicFile, int &error) {
        std::ifstream mapFile("./res/levels/" + mapLogicFile);
        std::string line;
        if (mapFile.is_open()) {
            int lineNumber = 0;
            while (getline(mapFile, line)) {
                if(lineNumber > 14) {
                    std::cerr << "The map height must be 15 or less, exiting.." << std::endl;
                    exit(-1);
                }
                //std::cout << line << '\n';

                for (unsigned int i = 0; i < line.length(); i++) {
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
        if(x <0 || x >= 224 || y < 0 ||y>=15) {
            std::cerr << "the requested object is out of map range " << x << ", " << y << std::endl;
            return GROUND;
        }
        return tiles[x][y];
    }

    /**
     * if returns -1,-1, it means not found.
     * @param type
     * @return
     */
    SDL_Rect getObject(TileTypes type) {
        SDL_Rect position;
        position.x = -1;
        position.y = -1;
        for (int i = 0; i < 224; i++) {
            for (int j = 0; j < 15; j++) {
                if (getTileObject(i,j) == type) {
                    position.x = i;
                    position.y = j;
                    return position;
                }
            }
        }
        return position;
    }

    SDL_Rect getAndRemoveObject(TileTypes types) {
        SDL_Rect rect = getObject(types);
        if(rect.x != -1 && rect.y != -1) {
            tiles[rect.x][rect.y] = TileTypes::EMPTY;
        }

        return rect;
    }
};


#endif //MARIO_MAP_H
