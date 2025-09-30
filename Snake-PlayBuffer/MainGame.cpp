#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#define _USE_MATH_DEFINES
#include "Play.h"
#include <math.h>

int DISPLAY_WIDTH = 256;
int DISPLAY_HEIGHT = 256;
int DISPLAY_SCALE = 2;

enum SPRITE {
	SPRITE_APPLE,
	SPRITE_EMPTY, // Probably won't be used but he's here anyway say hi
	SPRITE_BACK,
	SPRITE_BODY,
	SPRITE_HEAD
};

enum GAME_STATE {
	GAME_STATE_MENU,
	GAME_STATE_PLAYING,
	GAME_STATE_ENDED
};

enum DIRECTION {
	DIRECTION_RIGHT,
	DIRECTION_UP,
	DIRECTION_LEFT,
	DIRECTION_DOWN
};

int snakeLength = 3;
// index 0 = head, -1 = tail
vector<Play::Point2D> snakeCoords = { {7, 8}, {6, 8}, {5, 8} };
vector<int> snakeRotations = { DIRECTION_RIGHT, DIRECTION_RIGHT, DIRECTION_RIGHT };
Play::Point2D appleCoords = { 10, 8 };

int gameState = GAME_STATE_MENU;

int snakeDirection = DIRECTION_RIGHT;

float timeSinceMove = 0;


Play::Point2D CoordToPosition(Play::Point2D coord)
{
	return { coord.x * 16, coord.y * 16};
}

void KillPlayer() {
	gameState = GAME_STATE_ENDED;
	DrawDebugText({ 128, 128 }, "YOU LOST", Play::cBlue);
}

void SetupFrameBuffer() {
	Play::ClearDrawingBuffer(Play::cBlack);
	Play::DrawSprite(SPRITE_APPLE, CoordToPosition(appleCoords), 0);
	Play::DrawSpriteRotated(SPRITE_HEAD, CoordToPosition(snakeCoords[0]), 0, snakeRotations[0] * M_PI * 0.5);
	for (int i = 1; i < snakeCoords.size() - 1; i++) {
		Play::DrawSpriteRotated(SPRITE_BODY, CoordToPosition(snakeCoords[i]), 0, snakeRotations[i] * M_PI * 0.5);
	}
	int finalIndex = snakeCoords.size() - 1;
	Play::DrawSpriteRotated(SPRITE_BACK, CoordToPosition(snakeCoords[finalIndex]), 0, snakeRotations[finalIndex] * M_PI * 0.5);
}

void MovePlayer() {
	Play::Point2D newPosition = snakeCoords[0];

	if (snakeDirection == DIRECTION_DOWN) {
		newPosition.y -= 1;
	}
	if (snakeDirection == DIRECTION_UP) {
		newPosition.y += 1;
	}
	if (snakeDirection == DIRECTION_LEFT) {
		newPosition.x -= 1;
	}
	if (snakeDirection == DIRECTION_RIGHT) {
		newPosition.x += 1;
	}
	
	if (count(snakeCoords.begin(), snakeCoords.end(), newPosition)) {
		KillPlayer();
	}

	snakeCoords.insert(snakeCoords.begin(), newPosition);
	snakeRotations.insert(snakeRotations.begin(), snakeDirection);
	snakeRotations.resize(snakeLength);
	snakeCoords.resize(snakeLength);
}

void EatApple() {
	snakeLength++;
	while (true) {
		appleCoords = { Play::RandomRollRange(1, 15), Play::RandomRollRange(1, 15) };
		if (!count(snakeCoords.begin(), snakeCoords.end(), appleCoords)) {
			break;
		}
	}
}

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::CentreAllSpriteOrigins();
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	switch (gameState) {
	case GAME_STATE_MENU:
		//gameState == GAME_STATE_PLAYING; // This functionality will be implemented later :D
		//break;
	case GAME_STATE_PLAYING:
		SetupFrameBuffer();

		if (Play::KeyDown(KEY_DOWN)) {
			snakeDirection = DIRECTION_DOWN;
		}
		else if (Play::KeyDown(KEY_UP)) {
			snakeDirection = DIRECTION_UP;
		}
		else if (Play::KeyDown(KEY_LEFT)) {
			snakeDirection = DIRECTION_LEFT;
		}
		else if (Play::KeyDown(KEY_RIGHT)) {
			snakeDirection = DIRECTION_RIGHT;
		}

		if (timeSinceMove >= 0.5) {
			MovePlayer();
			timeSinceMove = 0;
		}

		timeSinceMove += elapsedTime;

		if (snakeCoords[0] == appleCoords) {
			EatApple();
		}
		Play::PresentDrawingBuffer();
		break;
	case GAME_STATE_ENDED:
		break;
	}
	return Play::KeyDown( KEY_ESCAPE );
}

// Gets called once when the player quits the game 
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}



