#pragma once

#include "aligned_box_collider.h"
#include "olcPixelGameEngine.h"

class Player {
public:
    olc::vf2d position; // bottom-left corner of the player
    olc::vf2d velocity;
    float health;
    float stunnedRemaining;

    static constexpr float width = 3.0f;
    static constexpr float height = 3.0f;
    static constexpr olc::vf2d size = { width, height };
    static constexpr float maxHealth = 100.0f;
    static constexpr float initialStunnedDuration = 0.3f;

    olc::vf2d crosshair; // center
    static constexpr float crosshairRadius = 1.0f;
    
    Player(float x, float y);

    AlignedBoxCollider getCollider() const;
    olc::vf2d getCenter() const;
    
    void takeDamage(float damage);
    bool isStunned() const;
};