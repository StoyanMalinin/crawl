#include <random>

class Random {
private:
    std::mt19937 rnd;
    
public:
    Random();
    Random(unsigned int s);

    void seed(unsigned int s);
    int getInt(int min, int max);
    bool getPercentChance(double percent);
    float getFloat(float min, float max);
};