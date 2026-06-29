#pragma once

#include "olcPixelGameEngine.h"
#include "aligned_box_collider.h"
#include "tile_type.h"

class AttackBall {
public:
    olc::vf2d position; // center of the ball
    olc::vf2d dir;

    static constexpr float radius = 0.5f;
    static constexpr float speed = 40.0f;
    static constexpr float damage = 5.0f;

    AttackBall() = default;
    AttackBall(olc::vf2d position, olc::vf2d dir);

    AlignedBoxCollider getCollider() const;

    static constexpr uint64_t collisionMask = (1ULL << uint64_t(TileType::Ground)) | (1ULL << uint64_t(TileType::FireBag));
};