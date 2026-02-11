#pragma once

#include "olcPixelGameEngine.h"
#include "aligned_box_collider.h"

class BallMonster {
public:
    size_t id;
    olc::vf2d position;

    static constexpr float radius = 5.0f;

    // These are for the box collider and the sprite
    static constexpr float width = radius * 2.0f;
    static constexpr float height = radius * 2.0f;

    BallMonster() = default;
    BallMonster(size_t id, olc::vf2d position);

    AlignedBoxCollider getCollider() const;
};