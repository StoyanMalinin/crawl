#pragma once

#include "aligned_box_collider.h"

class Player {
public:
    float x, y;
    static constexpr float width = 10.0f;
    static constexpr float height = 10.0f;
    
    Player(float x, float y) : x(x), y(y) {}

    AlignedBoxCollider getCollider() const;
};