#pragma once

#include "aligned_box_collider.h"
#include "olcPixelGameEngine.h"

class Player {
public:
    olc::vf2d position; // bottom-left corner of the player
    static constexpr float width = 5.0f;
    static constexpr float height = 5.0f;
    static constexpr olc::vf2d size = { width, height };
    
    Player(float x, float y) : position(x, y) {}

    AlignedBoxCollider getCollider() const;
};