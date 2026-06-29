#pragma once

#include <list>
#include "collisions.h"
#include "olcPixelGameEngine.h"

struct FireParticle {
    olc::vf2d position;
    olc::vf2d velocity;
    float lifetime;
    float size;

    olc::Pixel getColor() const {
        if (lifetime < 0.5f) return olc::GREY;
        else if (lifetime < 1.5f) return olc::RED;
        else if (lifetime < 2.0f) return olc::YELLOW;
        else return olc::WHITE;
    }

    AlignedBoxCollider getCollider() const {
        return AlignedBoxCollider(position.x, position.y, size, size);
    }
};

using FireParticleID = std::list<int>::iterator;

class FireBuffer {
    int capacity;
    FireParticle* particles;
    
    std::list<int> takenIndices;
    std::vector<int> freeIndices;
    
public:
    FireBuffer(int capacity);    
    FireBuffer(const FireBuffer&) = delete;
    FireBuffer& operator=(const FireBuffer&) = delete;

    FireParticleID addParticle(const FireParticle& particle);
    FireParticleID removeParticle(FireParticleID index);
    FireParticle getParticle(FireParticleID id);

    void moveParticles(float elapsedTime);

    std::list<int>& getTakenIndices();
};