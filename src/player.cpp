#include "player.h"

Player::Player(float x, float y) : position(x, y), velocity(0.0f, 0.0f), health(Player::maxHealth) {}

AlignedBoxCollider Player::getCollider() const
{
    return AlignedBoxCollider(position.x, position.y, width, height);
}

olc::vf2d Player::getCenter() const {
    return olc::vf2d(position.x + width / 2.0f, position.y + height / 2.0f);
}
