#include "ball_monster.h"
#include "aligned_box_collider.h"

BallMonster::BallMonster(size_t id, olc::vf2d position)
    : id(id), position(position) {}

AlignedBoxCollider BallMonster::getCollider() const {
    return AlignedBoxCollider(position.x, position.y, width, height);
}
