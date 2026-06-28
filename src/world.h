#include <map>
#include <cstdint>
#include "chunk.h"

class World {
private:
    std::map<int64_t, Chunk*> chunks;

public:
    constexpr static olc::vf2d worldSize = {Chunk::chunkWidth, Chunk::chunkHeight};

    Chunk& getChunkByID(int64_t chunkID);
    Chunk& getChunkByPosition(float x, float y);
    TileType getWorldAt(float x, float y);
    std::vector<Chunk*> getRelevantChunks(float minX, float minY, float maxX, float maxY);
};