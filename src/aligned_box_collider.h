#pragma once

#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"

class AlignedBoxCollider {
public:
    float x, y; // bottom-left corner of the box
    float width, height;
    
    AlignedBoxCollider(float x, float y, float width, float height);
    void debugDraw(olc::TransformedView tv, olc::Pixel color) const;
};