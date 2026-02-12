#pragma once

#include "olcPixelGameEngine.h"

class AttackBall {
public:
    olc::vf2d position; // center of the ball
    olc::vf2d dir;

    static constexpr float radius = 0.5f;
    static constexpr float speed = 40.0f;

    AttackBall() = default;
    AttackBall(olc::vf2d position, olc::vf2d dir);
};