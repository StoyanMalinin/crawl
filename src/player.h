#pragma once

#include "aligned_box_collider.h"
#include "olcPixelGameEngine.h"

class Player {
public:
    olc::vf2d position; // bottom-left corner of the player
    olc::vf2d velocity;
    static constexpr float width = 3.0f;
    static constexpr float height = 3.0f;
    static constexpr olc::vf2d size = { width, height };

    olc::vf2d crosshair; // center
    static constexpr float crosshairRadius = 1.0f;
    
    Player(float x, float y) : position(x, y), velocity(0.0f, 0.0f) {}

    AlignedBoxCollider getCollider() const;
    olc::vf2d getCenter() const;
};