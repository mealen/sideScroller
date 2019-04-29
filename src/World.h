//
// Created by engin on 11.11.2017.
//

#ifndef MARIO_WORLD_H
#define MARIO_WORLD_H


#include "Objects/InteractiveObject.h"
#include "Objects/Mario.h"
#include "Constants.h"
#include <memory>


class InputHandler;
class World {
public:
    enum class Sides{ LEFT, RIGHT, UP, DOWN, NONE};// none is not used, but here for checks

    struct PortalInformation {
        std::string worldName;
        bool startOverride;
        int startPosX;
        int startPosY;
        Sides startAnimationSide;
    };

    struct PlayerAnimation {
        int movementSpeed;
        int maximumMovement;
        int duration;
        Sides movementSide;
        std::shared_ptr<PlayerAnimation> nextAnimation = nullptr;
    };

private:
    struct Portal {
        int coordinates[4];//left, right, up, down
        Sides moveSide = Sides::NONE;
        std::string targetWorld;
        int startPosition[2];
        bool startOverride = false;
        Sides newWorldAnimSide = Sides::NONE;
    };
    std::vector<Portal> portals;
    std::vector<std::shared_ptr<InteractiveObject>> objects;
    SDL_Renderer *ren = nullptr;
    std::shared_ptr<Mario> mario = nullptr;

    long lastSetupTime = 0;
    TileTypes tiles[224][15];
    SDL_Texture *worldBackgroundImageTexture;
    SDL_Texture *worldForegroundImageTexture;
    uint32_t mapWidth;
    SDL_Rect worldRenderRectangle;
    Mix_Music *music;
    PortalInformation* activatedPortalInformation = nullptr;
    long animationAnimationStartTime = 0;
    bool portalTriggeredExternally = false;
    int animationStartPositionX, animationStartPositionY;
    const long PORTAL_ANIMATION_DURATION = 1000;
    std::shared_ptr<PlayerAnimation> currentAnimation = nullptr;

    void parseAdvancedFeatures(std::ifstream &mapfile);
    bool animatePlayer(long time) const;

public:
    SDL_Renderer *getRen() const {
        return ren;
    }
    void load(std::string worldName, int &error, Mix_Music *&music);
    TileTypes getTileObject(int x, int y);
    SDL_Rect getAndRemoveObject(TileTypes types);
    SDL_Rect getObject(TileTypes type);
    void addObject(std::shared_ptr<InteractiveObject> object) {
        objects.push_back(object);
    }

    uint32_t
    loadTexture(SDL_Renderer *ren, SDL_Texture *&worldImageTexture, uint32_t &mapWidth, const std::string &imageFile);

    int getWorldRenderX() {
        int retVal;
        AABB* marioPos = mario->getPosition();
        int middleOfScreenPixel = (SCREEN_WIDTH) / 2 - TILE_SIZE;

        if (marioPos->getMaxRight() - TILE_SIZE <= middleOfScreenPixel) {
            //if mario is not passed middle of the screen
            retVal = 0;
        } else {
            //put mario at middle of the screen, and move background to left
            //but first check if mario has been right before
            retVal = (marioPos->getMaxRight() - TILE_SIZE) - middleOfScreenPixel;
            if ((uint32_t) retVal > mapWidth - SCREEN_WIDTH) {
                //if end of map, let mario move more, and lock background
                retVal = mapWidth - SCREEN_WIDTH;
            }
        }

        return retVal;
    }
    /**
     *
     * @param ren  renderer
     * @param x -> current world x
     * @param y -> current world y
     * @param time -> get ticks
     */
    void render(SDL_Renderer *ren, long time) {
        //calculate x and y from mario position

        worldRenderRectangle.x = getWorldRenderX();

        SDL_RenderCopy(ren, worldBackgroundImageTexture, &worldRenderRectangle, NULL);

        for (unsigned int i = 0; i < objects.size(); ++i) {
            objects[i]->render(ren, worldRenderRectangle.x, worldRenderRectangle.y ,time);
        }
        mario->render(ren, worldRenderRectangle.x, worldRenderRectangle.y, time);
        SDL_RenderCopy(ren, worldForegroundImageTexture, &worldRenderRectangle, NULL);

    }

    /**
     *
     * Checks if the box is at collition with given tile coordinates.
     * The coordinates are not graphical, but logical.
     *
     * @param rightSpeed 1 for right, -1 for left, 0 for stopped
     * @param downSpeed 1 for up, -1 for down, 0 for stopped
     * @return true if collides
     */
    TileTypes collide(int rightSpeed, int downSpeed, long time, std::shared_ptr<Context> context,
                           std::shared_ptr<InteractiveObject> interactiveObject);

    void stepSimulation(long time, std::shared_ptr<Context> context);

    void stepSingleObject(long time, const std::shared_ptr<Context> &context,
                          const std::shared_ptr<InteractiveObject> interactiveObject);

    World(SDL_Renderer *ren) : ren(ren) {
        worldRenderRectangle.x = 0;
        worldRenderRectangle.y = 0;
        worldRenderRectangle.h = SCREEN_HEIGHT;
        worldRenderRectangle.w = SCREEN_WIDTH;
    }

    ~World(){
        SDL_DestroyTexture(worldBackgroundImageTexture);
        SDL_DestroyTexture(worldForegroundImageTexture);
        Mix_HaltMusic();
        Mix_FreeMusic(music);
    }

    void setMario(std::shared_ptr<Mario> mario) {
        this->mario = mario;
    }

    uint32_t getMapWidth() const {
        return mapWidth;
    }

    bool checkPortal(AABB *position, World::Sides side, PortalInformation** portalInformation);

    bool processInput(const InputHandler *input, long time, PortalInformation **portalInformation);

    void triggerPortalAnimation(World::Sides side, long time);

    void playAnimation(std::shared_ptr<PlayerAnimation> animation, Mario::TextureNames state);

};

#endif //MARIO_WORLD_H