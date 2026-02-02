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
#include <chrono>
#include <iostream>
#include "assets.h"

class Crawl : public olc::PixelGameEngine
{
public:
	Crawl() : assets("assets")
	{
		sAppName = "Crawl";
	}

private:
	Assets assets;
	olc::vf2d worldOffset = {0.0f, 0.0f};
	const olc::vf2d worldSize = {Chunk::chunkWidth, Chunk::chunkHeight};
public:
	bool OnUserCreate() override {		
		return true;
	}

	bool OnUserUpdate(float elapsedTime) override {
		if (GetKey(olc::Key::UP).bHeld) {
			worldOffset.y += 100.0f * elapsedTime;
		}
		if (GetKey(olc::Key::DOWN).bHeld) {
			worldOffset.y -= 100.0f * elapsedTime;
		}
		
		olc::TransformedView tv;
		tv.Initialise({ ScreenWidth(), ScreenHeight() });
		tv.SetWorldOffset(worldOffset);
		tv.SetWorldScale({ScreenWidth() / worldSize.x, ScreenHeight() / worldSize.y});

		int lChunkID = Chunk::yPositionToChunkID(worldOffset.y + 10.0f);
		int rChunkID = Chunk::yPositionToChunkID(worldOffset.y - worldSize.y - 10.0f);

		Clear(olc::WHITE);
		for (int chunkID = lChunkID; chunkID <= rChunkID; chunkID++) {
			Chunk chunk(chunkID);
			chunk.initialize();

			olc::vf2d offset = Chunk::chunkIDToOffset(chunkID);
			for (int x = 0; x < Chunk::chunkSizeX; x++) {
				for (int y = 0; y < Chunk::chunkSizeY; y++) {
					olc::Decal *decal = chunk.getMap(x, y) ? 
						assets.getDecal("wall.png") : 
						assets.getDecal("background.png");
					olc::vf2d scale = {
						Chunk::blockSizeX / decal->sprite->width,
						Chunk::blockSizeY / decal->sprite->height
					};

					tv.DrawDecal(
						offset + olc::vf2d(x * Chunk::blockSizeX, y * Chunk::blockSizeY),
						decal,
						scale
					);
				}
			}
		}

		return true;
	}
};


int main() {
	Crawl crawl;
	if (crawl.Construct(1280, 720, 1, 1))
		crawl.Start();
}
