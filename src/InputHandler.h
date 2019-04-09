//
// Created by engin on 9.04.2019.
//

#ifndef MARIO_INPUTHANDLER_H
#define MARIO_INPUTHANDLER_H

class InputHandler {
    public:
        bool quit = false;
        bool goRight = false;
        bool goLeft = false;
        bool crouch = false;
        bool crouchEvent = false;
        bool jump = false;
        bool jumpEvent = false;
        bool stop = false;
        bool run = false;
        bool restart = false;

    void readInput();
};


#endif //MARIO_INPUTHANDLER_H
