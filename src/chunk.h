#pragma once

#include "olcPixelGameEngine.h"
#include "ball_monster.h"
#include "fire.h"
#include "collisions.h"
#include "tile_type.h"

class Chunk {
private:
    int chunkID;
    TileType **map = nullptr;
    std::vector<BallMonster> ballMonsters;

    std::vector<std::vector<bool>> isIgnited;
    std::vector<std::vector<float>> burningTime;

private:
    void free();
    void copy(const Chunk& other);
    void seedChunk();
    void applyCaveAutomaton();
    static std::vector<olc::vf2d> getChunkPathInducingPoints(int chunkID);
    void generateBallMonsters();
    void generateFireBags();
    void generateTrees();
    bool checkRectContent(int x, int y, int width, int height, TileType type);
    
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
    AlignedBoxCollider getCollider() const;
    void debugDraw(olc::TransformedView tv) const;

    void ignite(int x, int y);
    bool isIgnitedAt(int x, int y) const;
    void updateBurning(float elapsedTime);
    void addParticlesToFireBuffer(float elapsedTime, FireBuffer& fireBuffer) const;

public:
	static constexpr int chunkSizeX = 60, chunkSizeY = 30; 
	static constexpr float chunkWidth = 128.0f;
	static constexpr float chunkHeight = 72.0f;
	static constexpr float blockSizeX = chunkWidth / float(chunkSizeX);
	static constexpr float blockSizeY = chunkHeight / float(chunkSizeY);
    static constexpr float minBurningTime = 2.0f, maxBurningTime = 4.0f; // seconds
    static constexpr float fireParticlesPerSecond = 500.0f;
    static constexpr int treeWidht = 2, treeHeight = 4;
};