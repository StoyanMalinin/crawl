#include "player.h"

Player::Player(float x, float y) : position(x, y), velocity(0.0f, 0.0f), 
    health(Player::maxHealth), stunnedRemaining(0), airbornDistance(0) {}

AlignedBoxCollider Player::getCollider() const
{
    return AlignedBoxCollider(position.x, position.y, width, height);
}

olc::vf2d Player::getCenter() const {
    return olc::vf2d(position.x + width / 2.0f, position.y + height / 2.0f);
}

void Player::takeDamage(float damage) {
    if (stunnedRemaining == 0.0f) {
        health = std::max(0.0f, health - damage);
        stunnedRemaining = initialStunnedDuration;     
    }
}

bool Player::isStunned() const {
    return stunnedRemaining > 0.0f;
}

void Player::takeFallDamage(float distance){
    if (distance > airbornDamageThreshold) {
        takeDamage(distance - airbornDamageThreshold); 
    }
}
