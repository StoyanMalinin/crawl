#include "world.h"

Chunk &World::getChunkByID(int64_t chunkID) {
    if (chunks.find(chunkID) == chunks.end()) {
        chunks[chunkID] = new Chunk(chunkID);
        chunks[chunkID]->initialize();
    }

    return *chunks[chunkID];
}

Chunk &World::getChunkByPosition(float x, float y) {
    int64_t chunkID = Chunk::yPositionToChunkID(y);
    return getChunkByID(chunkID);
}

TileType World::getWorldAt(float x, float y) {
    Chunk &chunk = getChunkByPosition(x, y);

    olc::vi2d gridPos = chunk.positionToGrid(x, y);
    return chunk.getMap(gridPos.x, gridPos.y);
}

std::vector<Chunk*> World::getRelevantChunks(float minX, float minY, float maxX, float maxY) {
    int64_t lChunkID = Chunk::yPositionToChunkID(maxY);
    int64_t rChunkID = Chunk::yPositionToChunkID(minY);

    std::vector<Chunk*> relevantChunks;
    for (int64_t chunkID = lChunkID; chunkID <= rChunkID; chunkID++) {
        relevantChunks.push_back(&getChunkByID(chunkID));
    }

    return relevantChunks;
}
