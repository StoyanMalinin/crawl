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
    const olc::vf2d chunkOffset = chunk.getOffset();
    const AlignedBoxCollider playerCollider = player.getCollider();
    playerCollider.debugDraw(tv, olc::GREEN);

    for (int x = 0; x < Chunk::chunkSizeX; x++) {
        for (int y = 0; y < Chunk::chunkSizeY; y++) {
            if (!chunk.getMap(x, y)) continue;

            AlignedBoxCollider blockCollider(
                chunkOffset.x + x * Chunk::blockSizeX,
                chunkOffset.y + y * Chunk::blockSizeY,
                Chunk::blockSizeX,
                Chunk::blockSizeY
            );
            blockCollider.debugDraw(tv, olc::RED);

            if (checkCollision(playerCollider, blockCollider)) {
                return true;
            }
        }
    }

    return false;
}
