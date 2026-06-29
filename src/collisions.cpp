#include "collisions.h"
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"

#include <string>
#include "chunk.h"

bool Collisions::checkCollision(const AlignedBoxCollider &a, const AlignedBoxCollider &b) {
    // Collision happens if projections overlap on BOTH axes
    bool xOverlap = (a.x < b.x + b.width) && (a.x + a.width > b.x);
    bool yOverlap = (a.y < b.y + b.height) && (a.y + a.height > b.y);

    return xOverlap && yOverlap;
}

bool Collisions::checkCollision(const Chunk &chunk, const Player &player, olc::TransformedView tv) {
    const AlignedBoxCollider playerCollider = player.getCollider();
    return checkCollision(chunk, playerCollider, Player::collisionMask);
}

bool Collisions::checkCollision(const Chunk &chunk, const AlignedBoxCollider &collider, uint64_t collisionMask) {
    olc::vi2d collisionGridPos = getCollision(chunk, collider, collisionMask);
    return collisionGridPos.x != -1;
}

olc::vi2d Collisions::getCollision(const Chunk &chunk, const AlignedBoxCollider &collider, uint64_t collisionMask) {
    if (!checkCollision(chunk.getCollider(), collider)) {
        return {-1, -1};
    }

    int startX = std::max(0, static_cast<int>((collider.x - chunk.getOffset().x) / Chunk::blockSizeX));
    int endX = std::min(chunk.chunkSizeX - 1, static_cast<int>((collider.x + collider.width - chunk.getOffset().x) / Chunk::blockSizeX));

    int startY = std::max(0, static_cast<int>((collider.y - chunk.getOffset().y) / Chunk::blockSizeY));
    int endY = std::min(chunk.chunkSizeY - 1, static_cast<int>((collider.y + collider.height - chunk.getOffset().y) / Chunk::blockSizeY));

    for (int x = startX; x <= endX; x++) {
        for (int y = startY; y <= endY; y++) {
            TileType tileType = chunk.getMap(x, y);
            if (((collisionMask >> uint64_t(tileType)) & 1) == 0) continue;

            AlignedBoxCollider blockCollider = chunk.getBlockCollider(x, y);

            if (checkCollision(collider, blockCollider)) {
                return {x, y};
            }
        }
    }

    return {-1, -1};
}

float Collisions::getRayIntersection(olc::vf2d origin, olc::vf2d direction, AlignedBoxCollider collider) {
    // Using the "slab method" for ray-box intersection
    olc::vf2d invDir = { 1.0f / direction.x, 1.0f / direction.y };

    float t1 = (collider.x - origin.x) * invDir.x;
    float t2 = (collider.x + collider.width - origin.x) * invDir.x;
    float t3 = (collider.y - origin.y) * invDir.y;
    float t4 = (collider.y + collider.height - origin.y) * invDir.y;

    float tmin = std::max(std::min(t1, t2), std::min(t3, t4));
    float tmax = std::min(std::max(t1, t2), std::max(t3, t4));

    if (tmax < 0 || tmin > tmax) {
        return std::numeric_limits<float>::max(); // No intersection
    }

    return tmin;
}

float Collisions::getRayIntersection(olc::vf2d origin, olc::vf2d direction, const Chunk &chunk) {
    float closestIntersection = std::numeric_limits<float>::max();
    for (int x = 0; x < Chunk::chunkSizeX; x++) {
        for (int y = 0; y < Chunk::chunkSizeY; y++) {
            if (chunk.getMap(x, y) == TileType::Empty) continue;

            AlignedBoxCollider blockCollider = chunk.getBlockCollider(x, y);

            float currentIntersection = getRayIntersection(origin, direction, blockCollider);
            if (currentIntersection < closestIntersection) {
                closestIntersection = currentIntersection;
            }
        }
    }

    return closestIntersection;
}
