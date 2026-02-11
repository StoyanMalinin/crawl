#include "player.h"

AlignedBoxCollider Player::getCollider() const {
    return AlignedBoxCollider(position.x, position.y, width, height);
}

olc::vf2d Player::getCenter() const {
    return olc::vf2d(position.x + width / 2.0f, position.y + height / 2.0f);
}
