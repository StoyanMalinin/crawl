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
#include "aligned_box_collider.h"

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
	const olc::vf2d gravity = {0.0f, -60.0f};

	// Layer 0 is the default layer, drawn on top (lowest index = rendered last)
	// Game layer has higher index, so it's rendered behind layer 0
	uint8_t gameLayer;
	const uint8_t debugLayer = 0; // Use layer 0 for debug (on top)
	std::map<size_t, BallMonster> ballMonsters;
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
		// Camera follows the player, so we need to update the world offset based on the player's position
		worldOffset.y = player.position.y - worldSize.y / 2.0f;

		updatePlayer(elapsedTime);
		updateBallMonsters(elapsedTime);
	}

	void updateBallMonsters(float elapsedTime) {
		olc::TransformedView tv = createTransformedView();

		// Spawn new ball monsters
		int64_t lChunkID = Chunk::yPositionToChunkID(worldOffset.y + worldSize.y + 10.0f);
		int64_t rChunkID = Chunk::yPositionToChunkID(worldOffset.y - 10.0f);
		for (int64_t chunkID = lChunkID; chunkID <= rChunkID; chunkID++) {
			Chunk chunk(chunkID);
			chunk.initialize();

			for (const BallMonster& ballMonster : chunk.getBallMonsters()) {
				if (!ballMonsters.count(ballMonster.id)) {
					ballMonsters[ballMonster.id] = ballMonster;
				}
			}
		}

		// Move existing ball monsters
		for (auto& [id, ballMonster] : ballMonsters) {
			olc::vf2d dir = player.getCenter() - ballMonster.getCenter();
			dir = dir.norm();
			
			int64_t lChunkID = Chunk::yPositionToChunkID(ballMonster.position.y + BallMonster::height + BallMonster::viewRange);
			int64_t rChunkID = Chunk::yPositionToChunkID(ballMonster.position.y - BallMonster::viewRange);
			
			int64_t intersectionChunkID = 0;
			float intersectionTime = std::numeric_limits<float>::max();
			for (int64_t chunkID = lChunkID; chunkID <= rChunkID; chunkID++) {
				Chunk chunk(chunkID);
				chunk.initialize();

				float currIntersectionTime = Collisions::getRayIntersection(ballMonster.getCenter(), dir, chunk);
				if (currIntersectionTime < intersectionTime) {
					intersectionTime = currIntersectionTime;
					intersectionChunkID = chunkID;
				}
			}
			
			tv.DrawLine(ballMonster.getCenter(), player.getCenter(), olc::RED);
			if (intersectionTime > dir.mag()) {
				ballMonster.position += dir.norm() * 10.0f * elapsedTime;
			} else {
				tv.FillCircle(ballMonster.getCenter() + dir * intersectionTime, 0.5f, olc::YELLOW);
			}
		}
	}

	void updatePlayer(float elapsedTime) {
		olc::vf2d playerAbsoluteChange = {0.0f, 0.0f};
		
		// Input
		if (GetKey(olc::Key::UP).bPressed) {
			if (isPlayerOnGround()) {
				player.velocity.y += 4000.0f * elapsedTime;
			}
		}
		if (GetKey(olc::Key::LEFT).bHeld) {
			playerAbsoluteChange.x -= 20.0f * elapsedTime;
		}
		if (GetKey(olc::Key::RIGHT).bHeld) {
			playerAbsoluteChange.x += 20.0f * elapsedTime;
		}

		olc::vf2d playerVelocity = player.velocity;
		playerVelocity += gravity * elapsedTime;
		
		// Horizontal
		olc::vf2d playerPositionBackup = player.position;
		player.position.x += playerVelocity.x * elapsedTime + playerAbsoluteChange.x;
		if (checkPlayerCollisions()) {
			player.position = playerPositionBackup;
			player.velocity.x = 0.0f;
		} else {
			player.velocity.x = playerVelocity.x;
		}
		
		// Vertical
		playerPositionBackup = player.position;
		player.position.y += playerVelocity.y * elapsedTime + playerAbsoluteChange.y;
		if (checkPlayerCollisions()) {
			player.position = playerPositionBackup;
			player.velocity.y = 0.0f;
		} else {
			player.velocity.y = playerVelocity.y;
		}
	}

	bool isPlayerOnGround() {
		AlignedBoxCollider playerGoundCollider = player.getCollider();
		playerGoundCollider.height = 0.1f; // Check for collisions just below the player
		playerGoundCollider.y -= 0.1f; // Move the collider down by its height to check for ground contact
		playerGoundCollider.x += 0.1f; // Add a small horizontal tolerance to allow for walking on slopes
		playerGoundCollider.width -= 0.2f; // Reduce the width by the same amount to keep the collider centered on the player

		int64_t lChunkID = Chunk::yPositionToChunkID(worldOffset.y + worldSize.y + 10.0f);
		int64_t rChunkID = Chunk::yPositionToChunkID(worldOffset.y - 10.0f);
		for (int64_t chunkID = lChunkID; chunkID <= rChunkID; chunkID++) {
			Chunk chunk(chunkID);
			chunk.initialize();

			if (Collisions::checkCollision(chunk, playerGoundCollider)) {
				return true;
			}
		}

		return false;
	}

	// TODO: Think about optimizations here:
	// - Maybe introduce a "chunk cache" that stores the chunks that are currently visible, so we don't have to initialize them every frame
	// - Maybe introduce some space partitioning structure to quickly rule out chunk blocks that are far away from the player
	bool checkPlayerCollisions() {
		int64_t lChunkID = Chunk::yPositionToChunkID(worldOffset.y + worldSize.y + 10.0f);
		int64_t rChunkID = Chunk::yPositionToChunkID(worldOffset.y - 10.0f);
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
		drawBallMonsters(tv);
	}

	void drawBallMonsters(olc::TransformedView tv) {
		for (const auto& [id, ballMonster] : ballMonsters) {
			olc::Decal *decal = assets.getDecal("ball_monster.png");
			olc::vf2d scale = {
				BallMonster::width / decal->sprite->width,
				BallMonster::height / decal->sprite->height
			};

			tv.DrawDecal(ballMonster.position + olc::vf2d(0, BallMonster::height), decal, scale);
		}
	}

	void drawPlayer(olc::TransformedView& tv) {
		olc::Decal *decal = assets.getDecal("wizzard.png");
		olc::vf2d scale = {
			Player::width / decal->sprite->width,
			Player::height / decal->sprite->height
		};

		tv.DrawDecal(player.position + olc::vf2d(0, Player::height), decal, scale);
	}

	void drawChunks(olc::TransformedView tv) {		
		int64_t lChunkID = Chunk::yPositionToChunkID(worldOffset.y + worldSize.y + 10.0f);
		int64_t rChunkID = Chunk::yPositionToChunkID(worldOffset.y - 10.0f);
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
