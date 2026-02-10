#include "aligned_box_collider.h"

#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"

AlignedBoxCollider::AlignedBoxCollider(float x, float y, float width, float height)
    : x(x), y(y), width(width), height(height) {}

void AlignedBoxCollider::debugDraw(olc::TransformedView tv, olc::Pixel color) const {
    tv.DrawRectDecal(olc::vf2d(x, y + height), olc::vf2d(width, height), color);
}
