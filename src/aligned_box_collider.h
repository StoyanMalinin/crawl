#pragma once

class AlignedBoxCollider {
public:
    float x, y; // bottom-left corner of the box
    float width, height;
    
    AlignedBoxCollider(float x, float y, float width, float height);
};