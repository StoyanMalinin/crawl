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
#include "player.h"
#include "collisions.h"

class Crawl : public olc::PixelGameEngine
{
public:
	Crawl() : assets("assets"), player(50.0f, 0.0f) {
		sAppName = "Crawl";
	}

private:
	Player player;
	Assets assets;
	olc::vf2d worldOffset = {0.0f, 0.0f};
	const olc::vf2d worldSize = {Chunk::chunkWidth, Chunk::chunkHeight};

	// Layer 0 is the default layer, drawn on top (lowest index = rendered last)
	// Game layer has higher index, so it's rendered behind layer 0
	uint8_t gameLayer;
	const uint8_t debugLayer = 0; // Use layer 0 for debug (on top)
public:
	bool OnUserCreate() override {	
		// Create game layer - higher index means drawn behind
		gameLayer = CreateLayer();
		EnableLayer(gameLayer, true);
		
		return true;
	}

	bool OnUserUpdate(float elapsedTime) override {
		SetDrawTarget(debugLayer);
		SetPixelMode(olc::Pixel::Mode::ALPHA);
		Clear(olc::BLANK);
		update(elapsedTime);
		
		SetDrawTarget(gameLayer);
		SetPixelMode(olc::Pixel::Mode::NORMAL);
		Clear(olc::BLACK);
		draw();

		return true;
	}

	void update(float elapsedTime) {
		worldOffset.y = player.position.y - worldSize.y / 2.0f;

		olc::vf2d playerPositionBackup = player.position;
		if (GetKey(olc::Key::UP).bHeld) {
			player.position.y += 100.0f * elapsedTime;
		}
		if (GetKey(olc::Key::DOWN).bHeld) {
			player.position.y -= 100.0f * elapsedTime;
		}
		if (GetKey(olc::Key::LEFT).bHeld) {
			player.position.x -= 100.0f * elapsedTime;
		}
		if (GetKey(olc::Key::RIGHT).bHeld) {
			player.position.x += 100.0f * elapsedTime;
		}
		if (checkPlayerCollisions()) {
			player.position = playerPositionBackup;
		}
	}

	// TODO: Think about optimizations here:
	// - Maybe introduce a "chunk cache" that stores the chunks that are currently visible, so we don't have to initialize them every frame
	// - Maybe introduce some space partitioning structure to quickly rule out chunk blocks that are far away from the player
	bool checkPlayerCollisions() {
		int64_t lChunkID = Chunk::yPositionToChunkID(worldOffset.y + 10.0f);
		int64_t rChunkID = Chunk::yPositionToChunkID(worldOffset.y - worldSize.y - 10.0f);
		for (int64_t chunkID = lChunkID; chunkID <= rChunkID; chunkID++) {
			Chunk chunk(chunkID);
			chunk.initialize();

			olc::TransformedView tv = createTransformedView();
			if (Collisions::checkCollision(chunk, player, tv)) {
				return true;
			}
		}

		return false;
	}

	olc::TransformedView createTransformedView() {
		olc::TransformedView tv;
		tv.Initialise({ ScreenWidth(), ScreenHeight() });
		tv.SetWorldOffset(worldOffset);
		tv.SetWorldScale({ScreenWidth() / worldSize.x, ScreenHeight() / worldSize.y});
		return tv;
	}

	void draw() {
		olc::TransformedView tv = createTransformedView();
		
		// The order is important here, as the player needs to be drawn on top of the chunks
		drawChunks(tv);
		drawPlayer(tv);
	}

	void drawPlayer(olc::TransformedView& tv) {
		olc::Decal *decal = assets.getDecal("wizzard.png");
		olc::vf2d scale = {
			Player::width / decal->sprite->width,
			Player::height / decal->sprite->height
		};

		tv.DrawDecal(player.position + olc::vf2d(0, Player::height), decal, scale);
	}

	void drawChunks(olc::TransformedView& tv) {		
		int64_t lChunkID = Chunk::yPositionToChunkID(worldOffset.y + 10.0f);
		int64_t rChunkID = Chunk::yPositionToChunkID(worldOffset.y - worldSize.y - 10.0f);
		for (int64_t chunkID = lChunkID; chunkID <= rChunkID; chunkID++) {
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
						offset + olc::vf2d(x * Chunk::blockSizeX, y * Chunk::blockSizeY) + olc::vf2d(0, Chunk::blockSizeY), // + olc::vf2d(0, blockSizeY) to draw from bottom-left corner
						decal,
						scale
					);
				}
			}
		}
	}
};


int main() {
	Crawl crawl;
	if (crawl.Construct(1280, 720, 1, 1))
		crawl.Start();
}
