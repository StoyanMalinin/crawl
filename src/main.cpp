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
#include "attack_ball.h"
#include <set>
#include "world.h"

class Crawl : public olc::PixelGameEngine
{
public:
	Crawl() : assets("assets"), player(50.0f, 0.0f), rnd(22) {
		sAppName = "Crawl";
	}

private:
	Assets assets;
	olc::vf2d worldOffset = {0.0f, 0.0f};
	const olc::vf2d worldSize = {Chunk::chunkWidth, Chunk::chunkHeight};
	const olc::vf2d gravity = {0.0f, -60.0f};
	Random rnd; // should only be used for non-procedural things

	// Layer 0 is the default layer, drawn on top (lowest index = rendered last)
	// UI layer is rendered below the debug layer but above the game layer
	// Game layer has highest index, so it's rendered behind everything
	uint8_t uiLayer;
	uint8_t gameLayer;
	const uint8_t debugLayer = 0; // Use layer 0 for debug (on top)

	// Entities
	Player player;
	std::set<size_t> deadBallMonsters;
	std::map<size_t, BallMonster> ballMonsters;
	std::map<size_t, AttackBall> attackBalls;

	World world;
public:
	bool OnUserCreate() override {	
		// Create UI layer - drawn above the game but below the debug layer
		uiLayer = CreateLayer();
		EnableLayer(uiLayer, true);

		// Create game layer - highest index means drawn behind everything
		gameLayer = CreateLayer();
		EnableLayer(gameLayer, true);
		
		return true;
	}

	bool OnUserUpdate(float elapsedTime) override {
		SetDrawTarget(debugLayer);
		SetPixelMode(olc::Pixel::Mode::ALPHA);
		Clear(olc::BLANK);
		update(elapsedTime);
		
		SetDrawTarget(uiLayer);
		SetPixelMode(olc::Pixel::Mode::ALPHA);
		Clear(olc::BLANK);
		drawUI();

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
		updateAttackBalls(elapsedTime);
		updateBallMonsters(elapsedTime);
	}

	void updateAttackBalls(float elapsedTime) {
		for (auto it = attackBalls.begin(); it != attackBalls.end(); ) {
			auto& [id, attackBall] = *it;
			attackBall.position += attackBall.dir * AttackBall::speed * elapsedTime;
			
			// Check collision with chunks
			Chunk &chunk = world.getChunkByPosition(attackBall.position.x, attackBall.position.y);
			if (Collisions::checkCollision(chunk, attackBall.getCollider())) {
				it = attackBalls.erase(it);
				continue;
			}

			// Check collision with ball monsters
			bool erased = false;
			for (auto& [monsterID, ballMonster] : ballMonsters) {
				if (Collisions::checkCollision(ballMonster.getCollider(), attackBall.getCollider())) {
					ballMonster.health -= AttackBall::damage;
					it = attackBalls.erase(it);
					erased = true;
					break;
				}
			}
			if (!erased) {
				++it;
			}
		}
	}

	void cleanupDeadBallMonsters() {
		for (auto it = ballMonsters.begin(); it != ballMonsters.end(); ) {
			auto& [id, ballMonster] = *it;
			if (ballMonster.health <= 0.0f) {
				deadBallMonsters.insert(id);
				it = ballMonsters.erase(it);
			} else {
				++it;
			}
		}
	}

	void spawnBallMonsters() {
		std::vector<Chunk*> chunks = world.getRelevantChunks(worldOffset.x, worldOffset.y - 10.0f, worldOffset.x + worldSize.x, worldOffset.y + worldSize.y + 10.0f);
		for (Chunk* chunk : chunks) {
			for (const BallMonster& ballMonster : chunk->getBallMonsters()) {
				if (!ballMonsters.count(ballMonster.id) && !deadBallMonsters.count(ballMonster.id)) {
					ballMonsters[ballMonster.id] = ballMonster;
				}
			}
		}
	}

	void updateBallMonsters(float elapsedTime) {
		olc::TransformedView tv = createTransformedView();
		
		cleanupDeadBallMonsters();
		spawnBallMonsters();

		const int auraCirclePointCount = 8;
		std::vector<olc::vf2d> playerAuraPoints;
		for (int i = 0; i < auraCirclePointCount; i++) {
			float angle = i * 2.0f * 3.14159f / auraCirclePointCount;
			playerAuraPoints.push_back(player.getCenter() + olc::vf2d(std::cos(angle), std::sin(angle)) * BallMonster::playerAuraRadius);
			playerAuraPoints.push_back(player.getCenter() + olc::vf2d(std::cos(angle), std::sin(angle)) * BallMonster::playerAuraRadius / 2);
		}
		// for (const auto& p: playerAuraPoints) {
		// 		tv.FillCircle(p, 0.1f, olc::GREEN);
		// }

		// Move existing ball monsters
		for (auto& [id, ballMonster] : ballMonsters) {
			// tv.DrawCircle(ballMonster.getCenter(), BallMonster::viewRange, olc::RED);
			
			std::vector<Chunk*> monsterChunks = world.getRelevantChunks(ballMonster.position.x, ballMonster.position.y - BallMonster::viewRange, 
				ballMonster.position.x, ballMonster.position.y + BallMonster::height + BallMonster::viewRange);

			auto checkReachable = [&](olc::vf2d origin, olc::vf2d target) {
				olc::vf2d dir = target - origin;
				
				olc::vf2d dirOrt = dir.perp().norm();
				olc::vf2d end1 = origin + dirOrt * BallMonster::radius * 0.9f;
				olc::vf2d end2 = origin - dirOrt * BallMonster::radius * 0.9f;

				// tv.DrawLine(origin, origin + dir, olc::RED);
				// tv.DrawLine(end1, end1 + dir, olc::WHITE);
				// tv.DrawLine(end2, end2 + dir, olc::WHITE);

				for (const Chunk* chunk : monsterChunks) {
					for (const auto &o: {origin, end1, end2}) {
						float intersectionTime = Collisions::getRayIntersection(o, dir, *chunk);
						if (intersectionTime < 1.0f - 0.001f) {
							return false; // Unreachable
						}
					}
				}

				return true; // Reachable
			};

			bool monsterMoved = false;
			if ((player.getCenter() - ballMonster.getCenter()).mag() <= BallMonster::viewRange) { // Move towards the player if they are within view range	
				const float monsterSpeed = rnd.getFloat(5.0f, 30.0f);
				if (checkReachable(ballMonster.getCenter(), player.getCenter())) {
					olc::vf2d dir = (player.getCenter() - ballMonster.getCenter()).norm();	
					ballMonster.moveBy(dir * monsterSpeed * elapsedTime);
					monsterMoved = true;
				} else {
					for (const auto& p: playerAuraPoints) {
						// Note: This idea can be extended further by building a sophisticated aura points set that the monster uses for pathfinding
						if (checkReachable(ballMonster.getCenter(), p) && checkReachable(p, player.getCenter())) {
							olc::vf2d dir = (p - ballMonster.getCenter()).norm();
							ballMonster.moveBy(dir * monsterSpeed * elapsedTime);
							
							monsterMoved = true;
							break;
						}
					}
				}
			}
			
			if (!monsterMoved) { // Move in a random direction if the player is out of view range
				auto currTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch().count();
				olc::vf2d dir = olc::vf2d(std::cos(currTime / 1000.0f), std::sin(currTime / 1000.0f)) * elapsedTime * rnd.getFloat(5.0f, 10.0f);
				
				if (checkReachable(ballMonster.getCenter(), ballMonster.getCenter() + dir)) {
					ballMonster.moveBy(dir);
					monsterMoved = true;
				}
			}
		}
	}

	void updatePlayer(float elapsedTime) {
		olc::vf2d playerAbsoluteChange = {0.0f, 0.0f};

		// Take fall damage if the player has fallen a certain distance
		if (isPlayerOnGround()) {
			player.takeFallDamage(player.airbornDistance);
			player.airbornDistance = 0.0f;
		}

		// Input
		if (GetKey(olc::Key::W).bPressed) {
			if (isPlayerOnGround()) {
				player.velocity.y += 30.0f;
			}
		}
		if (GetKey(olc::Key::A).bHeld) {
			playerAbsoluteChange.x -= 30.0f * elapsedTime;
		}
		if (GetKey(olc::Key::D).bHeld) {
			playerAbsoluteChange.x += 30.0f * elapsedTime;
		}
		
		// Crosshair
		olc::TransformedView tv = createTransformedView();
		olc::vi2d screenMousePos = GetMousePos();
		player.crosshair = tv.ScreenToWorld(screenMousePos);

		// Fire attack ball
		if (GetMouse(olc::Mouse::LEFT).bPressed) {
			olc::vf2d dir = (player.crosshair - player.getCenter()).norm();
			attackBalls[rnd.getID()] = AttackBall(player.getCenter(), dir);
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
			if (player.position.y < playerPositionBackup.y) {
				player.airbornDistance += playerPositionBackup.y - player.position.y;
			}
			player.velocity.y = playerVelocity.y;
		}

		// Stun update
		player.stunnedRemaining = std::max(0.0f, player.stunnedRemaining - elapsedTime);

		// Take potential damage from ball monsters
		if (!player.isStunned()) {
			for (const auto& [id, ballMonster] : ballMonsters) {
				if (Collisions::checkCollision(player.getCollider(), ballMonster.getCollider())) {
					player.takeDamage(BallMonster::contactDamage);
					break;
				}
			}
		}
	}

	bool isPlayerOnGround() {
		AlignedBoxCollider playerGoundCollider = player.getCollider();
		playerGoundCollider.height = 0.1f; // Check for collisions just below the player
		playerGoundCollider.y -= 0.1f; // Move the collider down by its height to check for ground contact
		playerGoundCollider.x += 0.1f; // Add a small horizontal tolerance to allow for walking on slopes
		playerGoundCollider.width -= 0.2f; // Reduce the width by the same amount to keep the collider centered on the player

		std::vector<Chunk*> chunks = world.getRelevantChunks(worldOffset.x, worldOffset.y - 10.0f, worldOffset.x + worldSize.x, worldOffset.y + worldSize.y + 10.0f);
		for (Chunk* chunk : chunks) {
			if (Collisions::checkCollision(*chunk, playerGoundCollider)) {
				return true;
			}
		}

		return false;
	}

	// TODO: Think about optimizations here:
	// - Maybe introduce some space partitioning structure to quickly rule out chunk blocks that are far away from the player
	bool checkPlayerCollisions() {
		std::vector<Chunk*> chunks = world.getRelevantChunks(worldOffset.x, worldOffset.y - 10.0f, worldOffset.x + worldSize.x, worldOffset.y + worldSize.y + 10.0f);
		for (Chunk* chunk : chunks) {
			olc::TransformedView tv = createTransformedView();
			if (Collisions::checkCollision(*chunk, player, tv)) {
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

	void drawUI() {
		const int barWidth = 200;
		const int barHeight = 20;
		const int padding = 10;
		const int barX = ScreenWidth() - barWidth - padding;
		const int barY = padding;

		float healthPercent = player.health / Player::maxHealth;
		int filledWidth = static_cast<int>(barWidth * healthPercent);

		FillRect(barX, barY, barWidth, barHeight, olc::DARK_RED);
		uint8_t r = static_cast<uint8_t>(255 * (1.0f - healthPercent));
		uint8_t g = static_cast<uint8_t>(255 * healthPercent);
		FillRect(barX, barY, filledWidth, barHeight, olc::Pixel(r, g, 0));
		DrawRect(barX, barY, barWidth, barHeight, olc::WHITE);

		std::string healthText = std::format("Health: {} / {}", static_cast<int>(player.health), static_cast<int>(Player::maxHealth));
		int textWidth = healthText.size() * 8;
		int textX = barX + (barWidth - textWidth) / 2;
		int textY = barY + (barHeight - 8) / 2;
		DrawString(textX, textY, healthText, olc::WHITE);
	}

	void draw() {
		olc::TransformedView tv = createTransformedView();
		
		// The order is important here, as the player needs to be drawn on top of the chunks
		drawChunks(tv);
		drawPlayer(tv);
		drawAttackBalls(tv);
		drawBallMonsters(tv);
	}

	void drawAttackBalls(olc::TransformedView tv) {
		for (const auto& [id, attackBall] : attackBalls) {
			olc::Decal *decal = assets.getDecal("wizzard-attack-ball.png");
			olc::vf2d scale = {
				AttackBall::radius * 2 / decal->sprite->width,
				AttackBall::radius * 2 / decal->sprite->height
			};

			tv.DrawDecal(attackBall.position + olc::vf2d(-AttackBall::radius, AttackBall::radius), decal, scale);
		}
	}

	void drawBallMonsters(olc::TransformedView tv) {
		for (const auto& [id, ballMonster] : ballMonsters) {
			// Draw sprite
			olc::Decal *decal = assets.getDecal("ball-monster-right.png");
			if (ballMonster.lastDirection.x < 0) {
				decal = assets.getDecal("ball-monster-left.png");
			}
			olc::vf2d scale = {
				BallMonster::width / decal->sprite->width,
				BallMonster::height / decal->sprite->height
			};
			tv.DrawDecal(ballMonster.position + olc::vf2d(0, BallMonster::height), decal, scale);
		
			// Draw health bar
			float healthPercent = ballMonster.health / BallMonster::maxHealth;
			olc::vf2d healthBarSize = {BallMonster::width, 0.5f};
			tv.FillRectDecal(ballMonster.position + olc::vf2d(0, BallMonster::height + 0.5f), healthBarSize, olc::RED);
			tv.FillRectDecal(ballMonster.position + olc::vf2d(0, BallMonster::height + 0.5f), olc::vf2d(healthBarSize.x * healthPercent, healthBarSize.y), olc::GREEN);
		}
	}

	void drawPlayer(olc::TransformedView& tv) {
		// Player sprite
		olc::Decal *decal = assets.getDecal("wizzard-right.png");
		if (player.crosshair.x < player.position.x) {
			decal = assets.getDecal("wizzard-left.png");
		}

		olc::vf2d scale = {
			Player::width / decal->sprite->width,
			Player::height / decal->sprite->height
		};
		
		olc::Pixel tint = olc::WHITE;
		if (player.isStunned()) tint = olc::YELLOW;

		tv.DrawDecal(player.position + olc::vf2d(0, Player::height), decal, scale, tint);

		// Player crosshair
		tv.DrawLineDecal(player.crosshair - olc::vf2d(Player::crosshairRadius, 0), player.crosshair + olc::vf2d(Player::crosshairRadius, 0), olc::RED);
		tv.DrawLineDecal(player.crosshair - olc::vf2d(0, Player::crosshairRadius), player.crosshair + olc::vf2d(0, Player::crosshairRadius), olc::RED);
	}

	void drawChunks(olc::TransformedView tv) {
		std::vector<Chunk*> chunks = world.getRelevantChunks(worldOffset.x, worldOffset.y - 10.0f, worldOffset.x + worldSize.x, worldOffset.y + worldSize.y + 10.0f);
		for (Chunk* chunk : chunks) {
			for (int x = 0; x < Chunk::chunkSizeX; x++) {
				for (int y = 0; y < Chunk::chunkSizeY; y++) {
					olc::Decal *decal = chunk->getMap(x, y) ? 
						assets.getDecal("wall-mid.png") : 
						assets.getDecal("background.png");
					olc::vf2d scale = {
						Chunk::blockSizeX / decal->sprite->width,
						Chunk::blockSizeY / decal->sprite->height
					};

					tv.DrawDecal(
						chunk->getOffset() + olc::vf2d(x * Chunk::blockSizeX, y * Chunk::blockSizeY) + olc::vf2d(0, Chunk::blockSizeY), // + olc::vf2d(0, blockSizeY) to draw from bottom-left corner
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
