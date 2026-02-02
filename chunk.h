#include "olcPixelGameEngine.h"

class Chunk {
private:
    int chunkID;
    bool **map = nullptr;

private:
    void free();
    void copy(const Chunk& other);
    void seedChunk();
    void applyCaveAutomaton();
    static std::vector<olc::vf2d> getChunkPathInducingPoints(int chunkID);

public:
    Chunk(int chunkID);
    ~Chunk();
    Chunk(const Chunk& other);
    Chunk& operator=(const Chunk& other);

    bool getMap(int x, int y) const;
    int getChunkID() const;
    void initialize();
    olc::vf2d getOffset();    
    static olc::vf2d chunkIDToOffset(int chunkID);
    static int64_t yPositionToChunkID(float yPos);

public:
	static constexpr int chunkSizeX = 60, chunkSizeY = 30; 
	static constexpr float chunkWidth = 128.0f;
	static constexpr float chunkHeight = 72.0f;
	static constexpr float blockSizeX = chunkWidth / float(chunkSizeX);
	static constexpr float blockSizeY = chunkHeight / float(chunkSizeY);
};