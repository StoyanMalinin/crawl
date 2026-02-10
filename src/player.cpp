#include "player.h"

AlignedBoxCollider Player::getCollider() const {
    return AlignedBoxCollider(x, y, width, height);
}
