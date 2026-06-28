#pragma once

#include "olcPixelGameEngine.h"
#include "ball_monster.h"

enum class TileType {
    Empty,
    Ground,
    FireBag
};

class Chunk {
private:
    int chunkID;
    TileType **map = nullptr;
    std::vector<BallMonster> ballMonsters;

private:
    void free();
    void copy(const Chunk& other);
    void seedChunk();
    void applyCaveAutomaton();
    static std::vector<olc::vf2d> getChunkPathInducingPoints(int chunkID);
    void generateBallMonsters();
    void generateFireBags();
    
public:
    Chunk(int chunkID);
    ~Chunk();
    Chunk(const Chunk& other);
    Chunk& operator=(const Chunk& other);

    TileType getMap(int x, int y) const;
    olc::vi2d positionToGrid(float x, float y) const;
    int getChunkID() const;
    void initialize();
    olc::vf2d getOffset() const;
    static olc::vf2d chunkIDToOffset(int chunkID);
    static int64_t yPositionToChunkID(float yPos);
    const std::vector<BallMonster>& getBallMonsters() const;
    AlignedBoxCollider getBlockCollider(int x, int y) const;
    void debugDraw(olc::TransformedView tv) const;

public:
	static constexpr int chunkSizeX = 60, chunkSizeY = 30; 
	static constexpr float chunkWidth = 128.0f;
	static constexpr float chunkHeight = 72.0f;
	static constexpr float blockSizeX = chunkWidth / float(chunkSizeX);
	static constexpr float blockSizeY = chunkHeight / float(chunkSizeY);
};