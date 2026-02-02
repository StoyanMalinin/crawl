#define OLC_PGE_APPLICATION
#define OLC_PGEX_TRANSFORMEDVIEW
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#pragma GCC diagnostic pop

#include "random.h"
#include <vector>
#include <array>

class Crawl : public olc::PixelGameEngine
{
private:
	bool **chunk = nullptr;
	const int chunkSizeX = 60, chunkSizeY = 30; 
	const float chunkWidth = 128.0f;
	const float chunkHeight = 72.0f;
	const float blockSizeX = chunkWidth / float(chunkSizeX);
	const float blockSizeY = chunkHeight / float(chunkSizeY);
public:
	Crawl()
	{
		sAppName = "Crawl";
	}

private:
	std::vector<olc::vf2d> inducingPoints, points;

	void initializeChunk(int chunkID) {
		if (chunk != nullptr) {
			for (int i = 0; i < chunkSizeY; i++)
				delete[] chunk[i];
			delete[] chunk;
		}
		
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

		inducingPoints = chunkPathInducingPoints;
		points = chunkPathPoints;

		Random rnd(chunkID);
		
		chunk = new bool*[chunkSizeY];
		for (int i = 0; i < chunkSizeY; i++)
			chunk[i] = new bool[chunkSizeX];

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
					chunk[y][x] = false;
				} else {
					chunk[y][x] = rnd.getChance(0.7f);
				}
			}
		}
	}

	void chunkCaveAutomaton() {
		bool **newChunk = new bool*[chunkSizeY];
		for (int i = 0; i < chunkSizeY; i++)
			newChunk[i] = new bool[chunkSizeX];

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
						else if (chunk[ny][nx]) {
							adjacentWalls++;
						}
					}
				}

				if (adjacentWalls >= 5)
					newChunk[y][x] = true;
				else
					newChunk[y][x] = false;
			}
		}

		for (int i = 0; i < chunkSizeY; i++)
			delete[] chunk[i];
		delete[] chunk;
		chunk = newChunk;
	}

	olc::vf2d chunkIDToOffset(int chunkID) {
		return {0, -float(chunkID) * chunkHeight};
	}

	std::vector<olc::vf2d> getChunkPathInducingPoints(int chunkID) {
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

public:
	bool OnUserCreate() override {		
		initializeChunk(0);	
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {
		olc::TransformedView tv;
		tv.Initialise({ ScreenWidth(), ScreenHeight() });
		const olc::vf2d offset = chunkIDToOffset(0);
		tv.SetWorldOffset(offset);
		olc::vf2d worldSize = {
			chunkWidth,
			chunkHeight,
		};
		
		// Calculate scale
		olc::vf2d scale = {
			ScreenWidth() / worldSize.x,
			ScreenHeight() / worldSize.y
		};
		
		tv.SetWorldScale(scale);

		if (GetKey(olc::Key::R).bPressed) initializeChunk(0);
		if (GetKey(olc::Key::C).bPressed) chunkCaveAutomaton();

		// called once per frame
		for (int x = 0; x < chunkSizeX; x++)
			for (int y = 0; y < chunkSizeY; y++)
				tv.FillRect(offset.x + x * blockSizeX, offset.y + y * blockSizeY, blockSizeX + 0.1f, blockSizeY + 0.1f, 
					chunk[y][x] ? olc::BLACK : olc::WHITE);

		for (const auto& p : points) {
			tv.FillCircle(p, 1.0f, olc::RED);
		}
		for (const auto& p : inducingPoints) {
			tv.FillCircle(p, 2.0f, olc::BLUE);
		}

		return true;
	}
};


int main() {
	Crawl crawl;
	if (crawl.Construct(1280, 720, 1, 1))
		crawl.Start();
}
