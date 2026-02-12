#include "random.h"

Random::Random() {}
Random::Random(unsigned int s) : rnd(s) {}

void Random::seed(unsigned int s) {
    rnd.seed(s);
}

int Random::getInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rnd);
}

bool Random::getChance(double c) {
    std::bernoulli_distribution dist(c);
    return dist(rnd);
}

float Random::getFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rnd);
}

size_t Random::getID() {
    return rnd();
}
