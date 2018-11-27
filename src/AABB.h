//
// Created by engin on 5.11.2017.
//

#ifndef MARIO_AABB_H
#define MARIO_AABB_H

#include <algorithm>

class AABB {
    int x1,y1, x2,y2;
public:
    enum PhysicsState { STATIC, KINEMATIC, DYNAMIC };

private:
    PhysicsState physicsState = STATIC;
    int maxRight = 0;
    bool hasJumped = false;
    bool hasDied = false;
    bool hasJumpTriggered = false;
    int upwardSpeed = 0;
    int upwardRequest = 0;
    int horizontalSpeed = 0;

public:

    PhysicsState getPhysicsState() const {
        return physicsState;
    }

    void setPhysicsState(PhysicsState physicsState) {
        AABB::physicsState = physicsState;
    }

    void moveRight(int amount) {
        //x1 += amount;
        //x2 += amount;
        //maxRight = std::max(x2, maxRight);
        horizontalSpeed = amount;
    }

    void moveLeft(int amount) {
        //x1 -= amount;
        //x2 -= amount;
        horizontalSpeed = -1 * amount;
    }

    void jump(int amount) {
        hasJumpTriggered = true;
        upwardRequest = amount;
        hasJumped = true;
    }

    int getLeftBorder() const {
        return x1;
    }

    int getRightBorder() const {
        return x2;
    }

    int getUpBorder() const {
        return y1;
    }

    int getDownBorder() const {
        return y2;
    }

    int getMaxRight() const {
        return maxRight;
    }

    bool isHasJumped() const {
        return hasJumped;
    }

    int getUpwardSpeed() const {
        return upwardSpeed;
    }

    void setUpwardSpeed(int upwardSpeed) {
        AABB::upwardSpeed = upwardSpeed;
    }

    void setLeftBorder(int x1) {
        AABB::x1 = x1;
    }

    void setUpBorder(int y1) {
        AABB::y1 = y1;
    }

    void setRightBorder(int x2) {
        maxRight = std::max(x2, maxRight);
        AABB::x2 = x2;
    }

    void setDownBorder(int y2) {
        AABB::y2 = y2;
    }

    void setHasJumped(bool hasJumped) {
        AABB::hasJumped = hasJumped;
    }

    bool isHasJumpTriggered() const {
        return hasJumpTriggered;
    }

    void setHasJumpTriggered(bool hasJumpTriggered) {
        AABB::hasJumpTriggered = hasJumpTriggered;
    }

    int getHorizontalSpeed() const {
        return horizontalSpeed;
    }

    void setHorizontalSpeed(int horizontalSpeed) {
        AABB::horizontalSpeed = horizontalSpeed;
    }

    int getUpwardRequest() const {
        return upwardRequest;
    }

    void setUpwardRequest(int upwardRequest) {
        AABB::upwardRequest = upwardRequest;
    }

    void die() {
        AABB::hasDied = true;
    }

    AABB(int left, int right, int up, int down) {
        x1 = left;
        x2 = right;
        y1 = up;
        y2 = down;
        maxRight = x2;
    }

    bool isHasDied() const {
        return hasDied;
    }

    void setHasDied(bool hasDied) {
        AABB::hasDied = hasDied;
    }
};

#endif //MARIO_AABB_H
