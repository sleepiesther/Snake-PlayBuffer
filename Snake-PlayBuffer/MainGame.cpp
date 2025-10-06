#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#define _USE_MATH_DEFINES
#include "Play.h"
#include <math.h>

// Allows for a 15*15 grid with each cell being 16*16 px.
int DISPLAY_WIDTH = 256;
int DISPLAY_HEIGHT = 256;
int DISPLAY_SCALE = 2;

enum SPRITE {
	SPRITE_APPLE,
	SPRITE_EMPTY, // Probably won't be used but he's here anyway say hi
	SPRITE_BACK,
	SPRITE_BODY = 6, // Back has 4 sprites for rotation, so the body enum is offset to account for that
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

int maxLength = 225;
int snakeLength = 3;
float moveTime = 0.5f;
bool pressed = false;

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

void HandleWraparound(Play::Point2D& coord)
{
	if (coord.x > 15)
	{
		coord.x = 1;
	}
	if (coord.y > 15)
	{
		coord.y = 1;
	}
	if (coord.x < 1)
	{
		coord.x = 16;
	}
	if (coord.y < 1)
	{
		coord.y = 16;
	}
}

void KillPlayer() {
	gameState = GAME_STATE_ENDED;
	DrawDebugText({ 128, 128 }, "YOU LOST", Play::cBlue);
}

void WinGame() {
	gameState = GAME_STATE_ENDED;
	DrawDebugText({ 128, 128 }, "YOU WIN", Play::cBlue);
}

void SetupFrameBuffer() {
	Play::ClearDrawingBuffer(Play::cBlack);
	Play::DrawSprite(SPRITE_APPLE, CoordToPosition(appleCoords), 0);
	Play::DrawSprite(SPRITE_HEAD + snakeRotations[0], CoordToPosition(snakeCoords[0]), 0);
	for (int i = 1; i < snakeCoords.size() - 1; i++) {
		Play::DrawSprite(SPRITE_BODY, CoordToPosition(snakeCoords[i]), 0);
	}
	int finalIndex = snakeCoords.size() - 1;
	Play::DrawSprite(SPRITE_BACK + snakeRotations[finalIndex - 1], CoordToPosition(snakeCoords[finalIndex]), 0);
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
	
	HandleWraparound(newPosition);

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
	string speedString = "<  " + std::to_string(moveTime).substr(0, 4) + " seconds  >";
	
	switch (gameState) {
	case GAME_STATE_MENU:
		SetupFrameBuffer();

		DrawDebugText({ 128, 150 }, "PRESS SPACE", Play::cBlue);
		DrawDebugText({ 190, 30 }, "GAME SPEED", Play::cRed);
		DrawDebugText({ 190, 10 }, speedString.c_str(), Play::cRed);

		if (Play::GetMouseButton(LEFT) && !pressed)
		{
			pressed = true;
			Play::Point2D mousePos = Play::GetMousePos();

			if (mousePos.x > 240 && mousePos.y < 15)
			{
				if (moveTime < 4.9)
				{
					moveTime += 0.1f;

				}
			}
			else if (mousePos.x > 120 && mousePos.x < 140 && mousePos.y < 15)
			{
				if (moveTime > 0.2f)
				{
					moveTime -= 0.1f;
				}
			}
		}
		if (!Play::GetMouseButton(LEFT))
		{
			pressed = false;
		}
		
		if (Play::KeyDown(KEY_SPACE)) {
			gameState = GAME_STATE_PLAYING;
			break;
		}

		break;
	case GAME_STATE_PLAYING:
		SetupFrameBuffer();
		
		if (snakeLength == maxLength) {
			WinGame();
		}

		if (Play::KeyDown(KEY_DOWN) && snakeDirection != DIRECTION_UP) {
			snakeDirection = DIRECTION_DOWN;
		}
		else if (Play::KeyDown(KEY_UP) && snakeDirection != DIRECTION_DOWN) {
			snakeDirection = DIRECTION_UP;
		}
		else if (Play::KeyDown(KEY_LEFT) && snakeDirection != DIRECTION_RIGHT) {
			snakeDirection = DIRECTION_LEFT;
		}
		else if (Play::KeyDown(KEY_RIGHT) && snakeDirection != DIRECTION_LEFT) {
			snakeDirection = DIRECTION_RIGHT;
		}

		if (timeSinceMove >= moveTime) {
			MovePlayer();
			timeSinceMove = 0;
		}

		timeSinceMove += elapsedTime;

		if (snakeCoords[0] == appleCoords) {
			EatApple();
		}
		break;
	case GAME_STATE_ENDED:
		break;
	}
	Play::PresentDrawingBuffer();
	return Play::KeyDown( KEY_ESCAPE );
}

// Gets called once when the player quits the game 
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}



