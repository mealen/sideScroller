//
// Created by engin on 18.02.2019.
//

#ifndef MARIO_BOXBASE_H
#define MARIO_BOXBASE_H


#include "InteractiveObject.h"

class BoxBase : public InteractiveObject {
protected:
    bool whileHit = false;
public:
    bool isWhileHit() const {
        return whileHit;
    }
};


#endif //MARIO_BOXBASE_H
