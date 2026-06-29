#include "chunk.h"
#include "random.h"
#include <vector>
#include <algorithm>
#include <array>
#include "olcPixelGameEngine.h"
#include <random>

void Chunk::free() {
    if (map != nullptr) {
        for (int i = 0; i < chunkSizeY; i++) {
            delete[] map[i];
        }
        delete[] map;
        map = nullptr;
    }
}

void Chunk::copy(const Chunk& other) {
    chunkID = other.chunkID;
    isIgnited = other.isIgnited;
    burningTime = other.burningTime;
    
    if (other.map != nullptr) {
        map = new TileType*[chunkSizeY];
        for (int i = 0; i < chunkSizeY; i++) {
            map[i] = new TileType[chunkSizeX];
            for (int j = 0; j < chunkSizeX; j++) {
                map[i][j] = other.map[i][j];
            }
        }
    } else {
        map = nullptr;
    }
}

Chunk::Chunk(int chunkID) : chunkID(chunkID) {
    map = new TileType*[chunkSizeY];
    for (int i = 0; i < chunkSizeY; i++) {
        map[i] = new TileType[chunkSizeX];
    }

    isIgnited.resize(chunkSizeY, std::vector<bool>(chunkSizeX, false));
    burningTime.resize(chunkSizeY, std::vector<float>(chunkSizeX, 0.0f));
}

Chunk::~Chunk() {
    free();
}

Chunk::Chunk(const Chunk& other) {
    copy(other);
}

Chunk& Chunk::operator=(const Chunk& other) {
    if (this != &other) {
        free();
        copy(other);
    }
    return *this;
}

TileType Chunk::getMap(int x, int y) const {
    return map[y][x];
}

void Chunk::ignite(int x, int y) {
    if (!isIgnited[y][x]) {
        isIgnited[y][x] = true;
        burningTime[y][x] = Random::instance().getFloat(minBurningTime, maxBurningTime);
    }
}

bool Chunk::isIgnitedAt(int x, int y) const {
    return isIgnited[y][x];
}

void Chunk::updateBurning(float elapsedTime) {
    for (int x = 0; x < chunkSizeX; x++) {
        for (int y = 0; y < chunkSizeY; y++) {
            if (isIgnited[y][x]) {
                burningTime[y][x] -= elapsedTime;
                if (burningTime[y][x] <= 0.0f) {
                    isIgnited[y][x] = false;
                    map[y][x] = TileType::Empty;
                }
            }
        }
    }
}

void Chunk::addParticlesToFireBuffer(float elapsedTime, FireBuffer &fireBuffer) const {
    auto velocityDistribution = std::normal_distribution<float>(std::numbers::pi / 2.0f, 1.0f);
    int totalParticlesToAdd = static_cast<int>(fireParticlesPerSecond * elapsedTime);
    
    for (int x = 0; x < chunkSizeX; x++) {
        for (int y = 0; y < chunkSizeY; y++) {
            if (isIgnited[y][x]) {
                for (int i = 0; i < totalParticlesToAdd; i++) {
                    FireParticle particle;
                    particle.position = olc::vf2d(
                        chunkIDToOffset(chunkID).x + x * blockSizeX + Random::instance().getFloat(0.0f, blockSizeX),
                        chunkIDToOffset(chunkID).y + y * blockSizeY + Random::instance().getFloat(0.0f, blockSizeY)
                    );
                    float angle = velocityDistribution(Random::instance().getUnderlyingGenerator());
                    particle.velocity = olc::vf2d(std::cos(angle), std::sin(angle)) * Random::instance().getFloat(0.5f, 2.0f);
                    particle.lifetime = Random::instance().getFloat(0.5f, 2.0f);
                    particle.size = Random::instance().getFloat(0.1f, 0.3f);
                                        
                    // Hack: preemptively move the particle to avoid it being added and immediately removed in the same frame due to collision with the source
                    particle.position += particle.velocity * Random::instance().getFloat(blockSizeX / 2.0f, 2.0f * blockSizeX);
                    
                    fireBuffer.addParticle(particle);
                }
            }
        }
    }
}

olc::vi2d Chunk::positionToGrid(float x, float y) const {
    return olc::vi2d(
        static_cast<int>(std::floor((x - getOffset().x) / blockSizeX)),
        static_cast<int>(std::floor((y - getOffset().y) / blockSizeY))
    );
}

int Chunk::getChunkID() const {
    return chunkID;
}

void Chunk::initialize() {
    seedChunk();
    for (int i = 0; i < 4; i++) applyCaveAutomaton();

    generateBallMonsters();
    generateFireBags();
}

void Chunk::generateBallMonsters() {
    constexpr int xLookupOffset = (BallMonster::width + blockSizeX - 1) / blockSizeX;
    constexpr int yLookupOffset = (BallMonster::height + blockSizeY - 1) / blockSizeY;

    Random rnd(chunkID);
    const int monsterCount = rnd.getInt(1, 1);
    for (int i = 0; i < monsterCount; i++) {
        for (int attempts = 0; attempts < 10; attempts++) {
            bool failed = false;
            int xLeft = rnd.getInt(0, chunkSizeX - 1);
            int yDown = rnd.getInt(0, chunkSizeY - 1);
            
            for (int x = xLeft; x <= xLeft + xLookupOffset && x < chunkSizeX; x++) {
                for (int y = yDown; y < yDown + yLookupOffset && y < chunkSizeY; y++) {
                    if (map[y][x] == TileType::Ground) {
                        failed = true;
                        break;   
                    }
                }
            }
            
            if (!failed) {
                olc::vf2d position = chunkIDToOffset(chunkID) + olc::vf2d(float(xLeft) * blockSizeX, float(yDown) * blockSizeY);
                ballMonsters.emplace_back(rnd.getID(), position);
                break;
            }
        }
    }
}

void Chunk::generateFireBags() {
    Random rnd(chunkID);

    for (int i = 1; i < chunkSizeY - 1; i++) {
        for (int j = 0; j < chunkSizeX; j++) {
            if (map[i][j] == TileType::Empty && map[i - 1][j] == TileType::Ground) {
                if (rnd.getChance(0.1f)) {
                    map[i][j] = TileType::FireBag;
                }
            }
        }
    }
}

olc::vf2d Chunk::getOffset() const {
    return chunkIDToOffset(chunkID);
}

olc::vf2d Chunk::chunkIDToOffset(int chunkID) {
    return {0, -float(chunkID) * chunkHeight};
}

int64_t Chunk::yPositionToChunkID(float yPos) {
    return -static_cast<int64_t>(std::floor(yPos / chunkHeight));
}

const std::vector<BallMonster> &Chunk::getBallMonsters() const {
    return ballMonsters;
}

AlignedBoxCollider Chunk::getBlockCollider(int x, int y) const {
    return AlignedBoxCollider(
        chunkIDToOffset(chunkID).x + x * blockSizeX,
        chunkIDToOffset(chunkID).y + y * blockSizeY,
        blockSizeX,
        blockSizeY
    );
}

AlignedBoxCollider Chunk::getCollider() const {
    olc::vf2d offset = chunkIDToOffset(chunkID);
    return AlignedBoxCollider(
        offset.x,
        offset.y,
        chunkWidth,
        chunkHeight
    );
}

void Chunk::debugDraw(olc::TransformedView tv) const {
    for (int x = 0; x < chunkSizeX; x++) {
        for (int y = 0; y < chunkSizeY; y++) {
            if (map[y][x] == TileType::Ground) {
                AlignedBoxCollider collider = getBlockCollider(x, y);
                collider.debugDraw(tv, olc::YELLOW);
            }
        }
    }
}

void Chunk::seedChunk() {
    std::vector<olc::vf2d> chunkPathInducingPoints;
    for (int id = chunkID - 1; id <= chunkID + 1; id++) {
        auto currChunkPathPoints = getChunkPathInducingPoints(id);
        chunkPathInducingPoints.insert(chunkPathInducingPoints.end(), currChunkPathPoints.begin(), currChunkPathPoints.end());
    }
    
    std::vector<olc::vf2d> chunkPathPoints;
    for (int i = 1; i < chunkPathInducingPoints.size() - 2; i++) {
        const std::array<float, 4> ts = {0.0f, 0.33f, 0.66f, 1.0f};
        for (float t: ts) {
            // Catmull–Rom spline
            float t2 = t * t;
            float t3 = t2 * t;
            olc::vf2d point = 0.5f * ((2.0f * chunkPathInducingPoints[i]) +
                (-chunkPathInducingPoints[i - 1] + chunkPathInducingPoints[i + 1]) * t +
                (2.0f * chunkPathInducingPoints[i - 1] - 5.0f * chunkPathInducingPoints[i] + 4.0f * chunkPathInducingPoints[i + 1] - chunkPathInducingPoints[i + 2]) * t2 +
                (-chunkPathInducingPoints[i - 1] + 3.0f * chunkPathInducingPoints[i] - 3.0f * chunkPathInducingPoints[i + 1] + chunkPathInducingPoints[i + 2]) * t3);
        
            chunkPathPoints.push_back(point);
        }
    }

    Random rnd(chunkID);

    for (int y = 0; y < chunkSizeY; y++) {
        for (int x = 0; x < chunkSizeX; x++) {
            olc::vf2d blockCenter = {
                (x + 0.5f) * blockSizeX,
                (y + 0.5f) * blockSizeY,
            };
            blockCenter += chunkIDToOffset(chunkID);
            
            const float potentialMaxDist = chunkWidth;
            float minDist = potentialMaxDist;
            for (int i = 0; i < chunkPathPoints.size() - 1; i++) {
                olc::vf2d p1 = chunkPathPoints[i];
                olc::vf2d p2 = chunkPathPoints[i + 1];
                
                // Calculate distance from block center to line segment p1-p2
                olc::vf2d dir = p2 - p1;
                float len2 = dir.mag2();
                float t = ((blockCenter - p1).dot(dir)) / len2;
                t = std::max(0.0f, std::min(1.0f, t));
                olc::vf2d projection = p1 + dir * t;
                float dist = (blockCenter - projection).mag();
                
                minDist = std::min(minDist, dist);
            }
            
            float distRatio = minDist / potentialMaxDist;
            if (distRatio < 0.04f) {
                map[y][x] = TileType::Empty;
            } else {
                map[y][x] = rnd.getChance(0.7f) ? TileType::Ground : TileType::Empty;
            }
        }
    }
}

void Chunk::applyCaveAutomaton() {
    TileType **newMap = new TileType*[chunkSizeY];
    for (int i = 0; i < chunkSizeY; i++)
        newMap[i] = new TileType[chunkSizeX];

    for (int y = 0; y < chunkSizeY; y++) {
        for (int x = 0; x < chunkSizeX; x++) {
            int adjacentWalls = 0;
            for (int oy = -1; oy <= 1; oy++) {
                for (int ox = -1; ox <= 1; ox++) {
                    if (oy == 0 && ox == 0) continue;
                    int nx = x + ox;
                    int ny = y + oy;
                    if (nx < 0 || ny < 0 || nx >= chunkSizeX || ny >= chunkSizeY) {
                        adjacentWalls++;
                    }
                    else if (map[ny][nx] == TileType::Ground) {
                        adjacentWalls++;
                    }
                }
            }

            if (adjacentWalls >= 5)
                newMap[y][x] = TileType::Ground;
            else
                newMap[y][x] = TileType::Empty;
        }
    }

    for (int i = 0; i < chunkSizeY; i++)
        delete[] map[i];
    delete[] map;
    map = newMap;
}

std::vector<olc::vf2d> Chunk::getChunkPathInducingPoints(int chunkID) {
    Random rnd(chunkID);
    olc::vf2d offset = chunkIDToOffset(chunkID);

    const size_t pointCount = 3;
    std::vector<olc::vf2d> points(pointCount);
    for (int i = 0; i < pointCount; i++) {
        points[i] = { rnd.getFloat(0.0f, chunkWidth), rnd.getFloat(0.0f, chunkHeight) };
        points[i] += offset;
    }

    std::sort(points.begin(), points.end(), [](const olc::vf2d& a, const olc::vf2d& b) {
        return a.y > b.y;
    });

    return points;
}
