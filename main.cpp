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

class Crawl : public olc::PixelGameEngine
{
private:
	bool **chunk = nullptr;
	const int chunkSizeX = 160, chunkSizeY = 90; 
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
	void initializeChunk() {
		if (chunk != nullptr) {
			for (int i = 0; i < chunkSizeY; i++)
				delete[] chunk[i];
			delete[] chunk;
		}

		Random rnd(22);
		
		chunk = new bool*[chunkSizeY];
		for (int i = 0; i < chunkSizeY; i++)
			chunk[i] = new bool[chunkSizeX];

		for (int y = 0; y < chunkSizeY; y++)
			for (int x = 0; x < chunkSizeX; x++)
				chunk[y][x] = rnd.getPercentChance(55.0);
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

	std::vector<olc::vf2d> getChunkPathPoints(int chunkID) {
		Random rnd(chunkID);
		olc::vf2d offset = chunkIDToOffset(chunkID);

		std::vector<olc::vf2d> points(3);
		for (int i = 0; i < 3; i++) {
			points[i] = { rnd.getFloat(0.0f, chunkWidth), -rnd.getFloat(0.0f, chunkHeight) };
			points[i] += offset;
		}

		return points;
	}

public:
	bool OnUserCreate() override {		
		initializeChunk();	
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {
		olc::TransformedView tv;
		tv.Initialise({ ScreenWidth(), ScreenHeight() });
		const olc::vf2d offset = {0.0f, -chunkHeight};
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

		if (GetKey(olc::Key::R).bPressed) initializeChunk();
		if (GetKey(olc::Key::C).bPressed) chunkCaveAutomaton();

		// called once per frame
		for (int x = 0; x < chunkSizeX; x++)
			for (int y = 0; y < chunkSizeY; y++)
				tv.FillRect(offset.x + x * blockSizeX, offset.y + y * blockSizeY, blockSizeX, blockSizeY, 
					chunk[y][x] ? olc::BLACK : olc::WHITE);

		return true;
	}
};


int main() {
	Crawl crawl;
	if (crawl.Construct(1280, 720, 1, 1))
		crawl.Start();
}
