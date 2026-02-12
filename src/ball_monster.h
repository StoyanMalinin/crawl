#pragma once

#include "olcPixelGameEngine.h"
#include "aligned_box_collider.h"

class BallMonster {
public:
    size_t id;
    olc::vf2d position;
    olc::vf2d lastDirection;

    static constexpr float radius = 1.5f;

    // These are for the box collider and the sprite
    static constexpr float width = radius * 2.0f;
    static constexpr float height = radius * 2.0f;
    static constexpr float viewRange = 40.0f;
    static constexpr float playerAuraRadius = 20.0f;

    BallMonster() = default;
    BallMonster(size_t id, olc::vf2d position);

    AlignedBoxCollider getCollider() const;
    olc::vf2d getCenter() const;
    void moveBy(olc::vf2d delta);
};