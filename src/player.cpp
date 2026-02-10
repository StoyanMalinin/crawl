#include "player.h"

AlignedBoxCollider Player::getCollider() const {
    return AlignedBoxCollider(position.x, position.y, width, height);
}
