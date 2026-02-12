#include "attack_ball.h"

AttackBall::AttackBall(olc::vf2d position, olc::vf2d dir) : position(position), dir(dir.norm()) {}
