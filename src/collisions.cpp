#include "collisions.h"
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"

#include <string>

bool Collisions::checkCollision(const AlignedBoxCollider &a, const AlignedBoxCollider &b) {
    // Collision happens if projections overlap on BOTH axes
    bool xOverlap = (a.x < b.x + b.width) && (a.x + a.width > b.x);
    bool yOverlap = (a.y < b.y + b.height) && (a.y + a.height > b.y);

    return xOverlap && yOverlap;
}

bool Collisions::checkCollision(const Chunk &chunk, const Player &player, olc::TransformedView tv) {
    const AlignedBoxCollider playerCollider = player.getCollider();
    return checkCollision(chunk, playerCollider);
}

bool Collisions::checkCollision(const Chunk &chunk, const AlignedBoxCollider &collider) {
    for (int x = 0; x < Chunk::chunkSizeX; x++) {
        for (int y = 0; y < Chunk::chunkSizeY; y++) {
            if (!chunk.getMap(x, y)) continue;

            AlignedBoxCollider blockCollider = chunk.getBlockCollider(x, y);

            if (checkCollision(collider, blockCollider)) {
                return true;
            }
        }
    }

    return false;
}

bool Collisions::getRayIntersection(olc::vf2d origin, olc::vf2d direction, AlignedBoxCollider collider, float &intersectionTime) {
    // Using the "slab method" for ray-box intersection
    olc::vf2d invDir = { 1.0f / direction.x, 1.0f / direction.y };

    float t1 = (collider.x - origin.x) * invDir.x;
    float t2 = (collider.x + collider.width - origin.x) * invDir.x;
    float t3 = (collider.y - origin.y) * invDir.y;
    float t4 = (collider.y + collider.height - origin.y) * invDir.y;

    float tmin = std::max(std::min(t1, t2), std::min(t3, t4));
    float tmax = std::min(std::max(t1, t2), std::max(t3, t4));

    if (tmax < 0 || tmin > tmax) {
        return false; // No intersection
    }

    intersectionTime = tmin;
    return true;
}

bool Collisions::getRayIntersection(olc::vf2d origin, olc::vf2d direction, const Chunk &chunk, float &intersectionTime) {
    float closestIntersection = std::numeric_limits<float>::max();
    bool hit = false;

    for (int x = 0; x < Chunk::chunkSizeX; x++) {
        for (int y = 0; y < Chunk::chunkSizeY; y++) {
            if (!chunk.getMap(x, y)) continue;

            AlignedBoxCollider blockCollider = chunk.getBlockCollider(x, y);

            float currentIntersection;
            if (getRayIntersection(origin, direction, blockCollider, currentIntersection)) {
                if (currentIntersection < closestIntersection) {
                    closestIntersection = currentIntersection;
                    hit = true;
                }
            }
        }
    }

    if (hit) {
        intersectionTime = closestIntersection;
    }
    return hit;
}
