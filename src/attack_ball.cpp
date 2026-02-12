#include "attack_ball.h"
#include "aligned_box_collider.h"

AttackBall::AttackBall(olc::vf2d position, olc::vf2d dir) : position(position), dir(dir.norm()) {}

AlignedBoxCollider AttackBall::getCollider() const {
    return AlignedBoxCollider(position.x - radius, position.y - radius, radius * 2, radius * 2);
}
