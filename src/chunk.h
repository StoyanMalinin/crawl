#pragma once

#include "olcPixelGameEngine.h"
#include "ball_monster.h"

class Chunk {
private:
    int chunkID;
    bool **map = nullptr;
    std::vector<BallMonster> ballMonsters;

private:
    void free();
    void copy(const Chunk& other);
    void seedChunk();
    void applyCaveAutomaton();
    static std::vector<olc::vf2d> getChunkPathInducingPoints(int chunkID);
    void generateBallMonsters();
    
public:
    Chunk(int chunkID);
    ~Chunk();
    Chunk(const Chunk& other);
    Chunk& operator=(const Chunk& other);

    bool getMap(int x, int y) const;
    int getChunkID() const;
    void initialize();
    olc::vf2d getOffset() const;
    static olc::vf2d chunkIDToOffset(int chunkID);
    static int64_t yPositionToChunkID(float yPos);
    const std::vector<BallMonster>& getBallMonsters() const;
    AlignedBoxCollider getBlockCollider(int x, int y) const;

public:
	static constexpr int chunkSizeX = 60, chunkSizeY = 30; 
	static constexpr float chunkWidth = 128.0f;
	static constexpr float chunkHeight = 72.0f;
	static constexpr float blockSizeX = chunkWidth / float(chunkSizeX);
	static constexpr float blockSizeY = chunkHeight / float(chunkSizeY);
};