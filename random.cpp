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

bool Random::getPercentChance(double percent) {
    std::bernoulli_distribution dist(percent / 100.0);
    return dist(rnd);
}

float Random::getFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rnd);
}