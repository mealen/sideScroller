#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <map>

/*
 * Get the resource path for resources located in res/subDir
 * It's assumed the project directory is structured like:
 * bin/
 *  the executable
 * res/
 *  Lesson1/
 *  Lesson2/
 *
 * Paths returned will be Lessons/res/subDir
 */
std::string getResourcePath(const std::string &subDir = "") {
    //We need to choose the path separator properly based on which
    //platform we're running on, since Windows uses a different
    //separator than most systems
#ifdef _WIN32
    const char PATH_SEP = '\\';
#else
    const char PATH_SEP = '/';
#endif
    //This will hold the base resource path: Lessons/res/
    //We give it static lifetime so that we'll only need to call
    //SDL_GetBasePath once to get the executable path
    static std::string baseRes;
    if (baseRes.empty()) {
        //SDL_GetBasePath will return NULL if something went wrong in retrieving the path
        char *basePath = SDL_GetBasePath();
        if (basePath) {
            baseRes = basePath;
            SDL_free(basePath);
        } else {
            std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
            return "";
        }
        //We replace the last bin/ with res/ to get the the resource path
        size_t pos = baseRes.rfind("bin");
        baseRes = baseRes.substr(0, pos) + "res" + PATH_SEP;
    }
    //If we want a specific subdirectory path in the resource directory
    //append it to the base path. This would be something like Lessons/res/Lesson0
    return subDir.empty() ? baseRes : baseRes + subDir + PATH_SEP;
}

class InputStates {
public:
    bool quit = false;
    bool goRight = false;
    bool goLeft = false;
};

class Map {
public:
    int map[90][15];
};

SDL_Texture *loadTexture(SDL_Renderer *ren, std::string imageName) {
    SDL_Texture *texture;
    SDL_Surface *marioSurface = SDL_LoadBMP(imageName.c_str());
    if (marioSurface == nullptr) {
        std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
        return NULL;
    }
    Uint32 transparentColor = SDL_MapRGB(marioSurface->format, 255, 0, 255);
    SDL_SetColorKey(marioSurface, SDL_TRUE, transparentColor);

    texture = SDL_CreateTextureFromSurface(ren, marioSurface);

    if (texture == nullptr) {
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return NULL;
    }
}

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

    Mario(SDL_Rect mapPosition, SDL_Renderer *ren, int screenWidth, int &error) : position(mapPosition),
                                                                                  screenWidth(screenWidth) {

        position.w = 32;
        position.h = 32;
        position.x *= 32;
        position.y *= 32;

        std::cout << position.x << ", " << position.y << std::endl;

        std::string marioImage = getResourcePath("mario") + "mario.bmp";
        textures[STAND].push_back(loadTexture(ren, marioImage));

        marioImage = getResourcePath("mario") + "mario_move0.bmp";
        textures[MOVE].push_back(loadTexture(ren, marioImage));

        marioImage = getResourcePath("mario") + "mario_move1.bmp";
        textures[MOVE].push_back(loadTexture(ren, marioImage));

        marioImage = getResourcePath("mario") + "mario_move2.bmp";
        textures[MOVE].push_back(loadTexture(ren, marioImage));

        if (textures[STAND][0] == nullptr ||
            textures[MOVE][0] == nullptr ||
            textures[MOVE][1] == nullptr ||
            textures[MOVE][2] == nullptr) {
            std::cerr << "Error loading Mario textures" << std::endl;
            error = 1;
            return;
        }

        error = 0;
    }

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

Map *loadMap(std::string mapLogicFile) {
    Map *currentMap = new Map();
    std::ifstream myfile("./res/levels/" + mapLogicFile);
    std::string line;
    if (myfile.is_open()) {
        int lineNumber = 0;
        while (getline(myfile, line)) {
            //std::cout << line << '\n';
            for (int i = 0; i < line.length(); i++) {
                currentMap->map[i][lineNumber] = line.at(i) - 48;
            }
            lineNumber++;
        }
        myfile.close();
    } else {
        return NULL;
    }
    return currentMap;
}

void readInput(InputStates &input) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        //If user closes the window
        if (e.type == SDL_QUIT) {
            input.quit = true;
        }
        //If user presses any key
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    input.quit = true;
                    break;
                case SDLK_d:
                    input.goRight = true;
                    break;
                case SDLK_a:
                    input.goLeft = true;
                    break;
            }
        }
        if (e.type == SDL_KEYUP) {
            switch (e.key.keysym.sym) {
                case SDLK_d:
                    input.goRight = false;
                    break;
                case SDLK_a:
                    input.goLeft = false;
                    break;
            }
        }
        //If user clicks the mouse
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            input.quit = true;
        }
    }
}

SDL_Rect getObject(Map map, int objectId) {
    SDL_Rect position;
    position.x = 0;
    position.y = 0;
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 90; j++) {
            if (map.map[i][j] == objectId) {
                position.x = i;
                position.y = j;
                break;
            }
        }
    }
    return position;
}

int main(int argc, char *argv[]) {//these parameters has to be here or SDL_main linking issue arises
    std::cout << "Hello, World!" << std::endl;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (win == nullptr) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == nullptr) {
        SDL_DestroyWindow(win);
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    std::string imagePath = getResourcePath("levels") + "0101_graph.bmp";
    SDL_Surface *bmp = SDL_LoadBMP(imagePath.c_str());
    if (bmp == nullptr) {
        std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);

    if (tex == nullptr) {
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    int mapWidth = bmp->w;
    SDL_FreeSurface(bmp);
    SDL_Rect sourceRect;

    sourceRect.x = 0;
    sourceRect.y = 0;
    sourceRect.h = 480;
    sourceRect.w = 640;

    Map *map0101 = loadMap("0101_logic.txt");

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) { return false; }
    Mix_Music *music = NULL;
    music = Mix_LoadMUS("./res/sounds/overworld.wav");
    //If there was a problem loading the music
    if (music == NULL) {
        return false;
    }

    //Play the music
    if (Mix_PlayMusic(music, -1) == -1) {
        return 1;
    }

    InputStates input;
    input.quit = false;
    int error;
    Mario mario(getObject(*map0101, 8), ren, 640, error);
    if (error != 0) {
        std::cerr << "Error initializing Mario, Exiting" << std::endl;
        return -1;
    }


    SDL_RendererFlip leftRightFlip = SDL_FLIP_NONE;
    long time;
    SDL_Rect marioPos = mario.getPositionRect();;
    while (!input.quit) {
        time = SDL_GetTicks();
        readInput(input);
        //First clear the renderer
        SDL_RenderClear(ren);
        //Take a quick break after all that hard work
        //SDL_Delay(50);

        if (input.goRight) {
            mario.move(false, true, false, false);
            marioPos = mario.getPositionRect();
            leftRightFlip = SDL_FLIP_NONE;
            if (marioPos.x >= (640 - 64) / 2) {
                sourceRect.x += 3;
                if (sourceRect.x > mapWidth - 640) {
                    sourceRect.x = mapWidth - 640;
                }

            }
            //Draw the texture
            SDL_RenderCopy(ren, tex, &sourceRect, NULL);

            //draw the mario
            SDL_RenderCopyEx(ren, mario.getTexture(mario.MOVE, time), 0, &marioPos, 0, 0, leftRightFlip);

        } else if (input.goLeft) {
            mario.move(true, false, false, false);
            marioPos = mario.getPositionRect();
            leftRightFlip = SDL_FLIP_HORIZONTAL;
            if (marioPos.x < 0) {
                marioPos.x = 0;
            }
            SDL_RenderCopy(ren, tex, &sourceRect, NULL);
            SDL_RenderCopyEx(ren, mario.getTexture(mario.MOVE, time), 0, &marioPos, 0, 0, leftRightFlip);
        } else {
            SDL_RenderCopy(ren, tex, &sourceRect, NULL);
            SDL_RenderCopyEx(ren, mario.getTexture(mario.STAND, time), 0, &marioPos, 0, 0, leftRightFlip);
        }
        //Update the screen
        SDL_RenderPresent(ren);


    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    //return 0;
}



