#include "ball_monster.h"
#include "aligned_box_collider.h"

BallMonster::BallMonster(size_t id, olc::vf2d position)
    : id(id), position(position), health(BallMonster::maxHealth) {}

AlignedBoxCollider BallMonster::getCollider() const {
    return AlignedBoxCollider(position.x, position.y, width, height);
}

olc::vf2d BallMonster::getCenter() const {
    return olc::vf2d(position.x + width / 2.0f, position.y + height / 2.0f);
}

void BallMonster::moveBy(olc::vf2d delta) {
    position += delta;
    lastDirection = delta;
}
