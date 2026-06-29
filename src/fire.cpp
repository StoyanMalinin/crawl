#include "fire.h"

#include "random.h"

FireBuffer::FireBuffer(int capacity) {
    particles = new FireParticle[capacity];
    this->capacity = capacity;
    
    for (int i = 0; i < capacity; ++i) {
        freeIndices.push_back(i);
    }
}

FireParticleID FireBuffer::addParticle(const FireParticle &particle)
{
    int index;
    if (!freeIndices.empty()) {
        index = freeIndices.back();
        freeIndices.pop_back();
    } else {
        index = takenIndices.front();
        takenIndices.pop_front();    
    }

    particles[index] = particle;
    takenIndices.push_back(index);

    return std::prev(takenIndices.end());
}

FireParticleID FireBuffer::removeParticle(FireParticleID index) {
    freeIndices.push_back(*index);
    FireParticleID nextID = takenIndices.erase(index);

    return nextID;
}

FireParticle FireBuffer::getParticle(FireParticleID id) {
    return particles[*id];
}

void FireBuffer::moveParticles(float elapsedTime) {
    for (int index : takenIndices) {
        particles[index].velocity += olc::vf2d(0.0f, 5.0f) * elapsedTime; // hot air going up
        particles[index].position += particles[index].velocity * elapsedTime;
        particles[index].lifetime -= elapsedTime;
    }
    for (auto it = takenIndices.begin(); it != takenIndices.end(); ) {
        if (particles[*it].lifetime <= 0.0f) {
            it = removeParticle(it);
        } else {
            ++it;
        }
    }
}

std::list<int> &FireBuffer::getTakenIndices() {
    return takenIndices;
}
