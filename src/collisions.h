#pragma once

#include "aligned_box_collider.h"
#include "player.h"

#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "tile_type.h"

class Chunk;

class Collisions {
public:
    static bool checkCollision(const AlignedBoxCollider& a, const AlignedBoxCollider& b);
    static bool checkCollision(const Chunk& chunk, const Player& player, olc::TransformedView tv);
    static bool checkCollision(const Chunk& chunk, const AlignedBoxCollider& collider, uint64_t collisionMask);
    static olc::vi2d getCollision(const Chunk& chunk, const AlignedBoxCollider& collider, uint64_t collisionMask);

    static float getRayIntersection(olc::vf2d origin, olc::vf2d direction, AlignedBoxCollider collider);
    static float getRayIntersection(olc::vf2d origin, olc::vf2d direction, const Chunk& chunk);
};