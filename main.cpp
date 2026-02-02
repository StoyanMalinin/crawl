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
#include "chunk.h"

class Crawl : public olc::PixelGameEngine
{
public:
	Crawl()
	{
		sAppName = "Crawl";
	}

private:

public:
	bool OnUserCreate() override {		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {
		Chunk chunk(0);
		chunk.initialize();

		olc::TransformedView tv;
		tv.Initialise({ ScreenWidth(), ScreenHeight() });
		const olc::vf2d offset = {0.0f, 0.0f};
		tv.SetWorldOffset(offset);
		olc::vf2d worldSize = {
			Chunk::chunkWidth,
			Chunk::chunkHeight,
		};
		
		// Calculate scale
		olc::vf2d scale = {
			ScreenWidth() / worldSize.x,
			ScreenHeight() / worldSize.y
		};
		
		tv.SetWorldScale(scale);

		// called once per frame
		for (int x = 0; x < Chunk::chunkSizeX; x++)
			for (int y = 0; y < Chunk::chunkSizeY; y++)
				tv.FillRect(offset.x + x * Chunk::blockSizeX, offset.y + y * Chunk::blockSizeY, Chunk::blockSizeX + 0.1f, Chunk::blockSizeY + 0.1f, 
					chunk.getMap(x, y) ? olc::BLACK : olc::WHITE);

		return true;
	}
};


int main() {
	Crawl crawl;
	if (crawl.Construct(1280, 720, 1, 1))
		crawl.Start();
}
