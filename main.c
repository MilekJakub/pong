#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "raylib.h"

//----------------------------------------------------------------------------------
// Types and Structures
//----------------------------------------------------------------------------------

typedef struct Player {
    Vector2 position;
    Vector2 size;
    int points;
} Player;

typedef struct Ball {
	Vector2 position;
	Vector2 size;
    Vector2 speed;
} Ball;

//----------------------------------------------------------------------------------
// Global Variables
//----------------------------------------------------------------------------------

const int screenWidth = 800;
const int screenHeight = 600;
const int playerSpeed = 400;
const int ballSpeed = 700;
const int paddleMargin = 20;

bool isInMenu = true;
bool gameInitialized = false;
bool gameOver = false;
bool pause = false;
bool roundStart = true;
bool shouldExit = false;

float dt;
int menuOption = 1;

static Player player1 = { 0 };
static Player player2 = { 0 };
static Ball ball = { 0 };

//------------------------------------------------------------------------------------
// Module Functions
//------------------------------------------------------------------------------------

void InitGame(void);
void UpdateGame(void);
void DrawGame(void);
void MenuUpdate();
void GameplayUpdate(void);

bool CoinFlip(void);
bool PlayerCollideWithBall(Player *p, Ball *b);

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Pong");
    SetTargetFPS(60);
	SetExitKey(KEY_NULL);

    while (!shouldExit)
    {
		if (WindowShouldClose())
		{
			shouldExit = true;
		}

		if (!gameInitialized)
		{
			InitGame();
			gameInitialized = true;
		}

        dt = GetFrameTime();
		UpdateGame();
		DrawGame();
    }

    CloseWindow();
    return 0;
}

void InitGame(void)
{
    player1.size = (Vector2) { 20, 100 };
    player1.position = (Vector2) { (float)paddleMargin, screenHeight / 2 - (player1.size.y) / 2 };
	player1.points = 0;

    player2.size = (Vector2) { 20, 100 };
    player2.position = (Vector2) { screenWidth - player2.size.x - paddleMargin, screenHeight / 2 - (player2.size.y) / 2 };
	player2.points = 0;

    ball.size = (Vector2) { 10, 10 };
    ball.position = (Vector2) { screenWidth / 2 - ball.size.x / 2, screenHeight / 2 - ball.size.y / 2 };

    if (CoinFlip())
    {
        ball.speed.x = (float)-ballSpeed;
        ball.speed.y = 0;
    }

    else
    {
        ball.speed.x = (float)ballSpeed;
        ball.speed.y = 0;
    }
}


void UpdateGame(void)
{
    if (IsKeyPressed('P')) pause = !pause;

    if (isInMenu)
    {
        MenuUpdate();
    }
    else
    {
        if (!pause)
        {
			if (!roundStart)
			{
				GameplayUpdate();
				if (gameOver)
				{
					if (IsKeyPressed(KEY_ESCAPE))
					{
						InitGame();
						gameOver = false;
						isInMenu = true;
					}
				}
			}
			else
			{
				if (IsKeyPressed(KEY_SPACE))
				{
					roundStart = !roundStart;
				}
			}
        }
    }
}

void DrawGame(void)
{
    if (isInMenu)
    {
		BeginDrawing();

			ClearBackground(BLACK);
			DrawText("PONG", screenWidth / 2 - MeasureText("PONG", 40) / 2, 80, 40, GRAY);
			
			if (menuOption == 1)
			{
				DrawText("- PLAY -", screenWidth / 2 - MeasureText("- PLAY -", 40) / 2, screenHeight / 2 - 40, 40, WHITE);
				DrawText("EXIT", screenWidth / 2 - MeasureText("EXIT", 40) / 2, screenHeight / 2, 40, WHITE);
			}
			else
			{
				DrawText("PLAY", screenWidth / 2 - MeasureText("PLAY", 40) / 2, screenHeight / 2 - 40, 40, WHITE);
				DrawText("- EXIT -", screenWidth / 2 - MeasureText("- EXIT -", 40) / 2, screenHeight / 2, 40, WHITE);
			}

		EndDrawing();
    }
	else if (pause)
	{
		BeginDrawing();

			ClearBackground(BLACK);
			DrawRectangle((int)player1.position.x, (int)player1.position.y, (int)player1.size.x, (int)player1.size.y, RAYWHITE);
			DrawRectangle((int)player2.position.x, (int)player2.position.y, (int)player2.size.x, (int)player2.size.y, RAYWHITE);
			DrawRectangle((int)ball.position.x, (int)ball.position.y, (int)ball.size.x, (int)ball.size.y, RED);
			DrawText(TextFormat("%i", player1.points), 0 + MeasureText("SCORE 00", 40) / 2, screenHeight / 2 - 20, 40, GRAY);
			DrawText(TextFormat("%i", player2.points), screenWidth - MeasureText("SCORE 00", 40) / 2 - 40, screenHeight / 2 - 20, 40, GRAY);
			DrawText("PAUSE", screenWidth / 2 - MeasureText("PAUSE", 40) / 2, screenHeight / 2 - 20, 40, GRAY);

		EndDrawing();
	}
	else if (roundStart)
	{
		BeginDrawing();

			ClearBackground(BLACK);
			DrawRectangle((int)player1.position.x, (int)player1.position.y, (int)player1.size.x, (int)player1.size.y, RAYWHITE);
			DrawRectangle((int)player2.position.x, (int)player2.position.y, (int)player2.size.x, (int)player2.size.y, RAYWHITE);
			DrawRectangle((int)ball.position.x, (int)ball.position.y, (int)ball.size.x, (int)ball.size.y, RED);
			DrawText(TextFormat("%i", player1.points), 0 + MeasureText("SCORE 00", 40) / 2, screenHeight / 2 - 20, 40, GRAY);
			DrawText(TextFormat("%i", player2.points), screenWidth - MeasureText("SCORE 00", 40) / 2 - 40, screenHeight / 2 - 20, 40, GRAY);
			DrawText("PRESS SPACE", screenWidth / 2 - MeasureText("PRESS SPACE", 40) / 2, screenHeight / 2 - 80, 40, GRAY);
			DrawText("P1", 0 + MeasureText("P1", 40) / 2, screenHeight / 2 - 160, 40, GRAY);
			DrawText("P2", screenWidth - MeasureText("P2", 40) / 2 - 40, screenHeight / 2 - 160, 40, GRAY);

		EndDrawing();
	}
	else if (gameOver)
	{
		BeginDrawing();

			ClearBackground(BLACK);
			DrawRectangle((int)player1.position.x, (int)player1.position.y, (int)player1.size.x, (int)player1.size.y, RAYWHITE);
			DrawRectangle((int)player2.position.x, (int)player2.position.y, (int)player2.size.x, (int)player2.size.y, RAYWHITE);
			if (player1.points == 10)
			{
				DrawText("P1 WINS!", screenWidth / 2 - MeasureText("P1 WINS!", 40) / 2, screenHeight / 2 - 20, 40, GRAY);
			}
			else
			{
				DrawText("P2 WINS!", screenWidth / 2 - MeasureText("P2 WINS!", 40) / 2, screenHeight / 2 - 20, 40, GRAY);
			}
			DrawText("PRESS ESC", screenWidth / 2 - MeasureText("PRESS ESC", 40) / 2, screenHeight / 2 + 40, 40, GRAY);
			DrawText("P1", 0 + MeasureText("P1", 40) / 2, screenHeight / 2 - 160, 40, GRAY);
			DrawText("P2", screenWidth - MeasureText("P2", 40) / 2 - 40, screenHeight / 2 - 160, 40, GRAY);

		EndDrawing();
	}
	else
    {
		BeginDrawing();

			ClearBackground(BLACK);
			DrawRectangle((int)player1.position.x, (int)player1.position.y, (int)player1.size.x, (int)player1.size.y, RAYWHITE);
			DrawRectangle((int)player2.position.x, (int)player2.position.y, (int)player2.size.x, (int)player2.size.y, RAYWHITE);
			DrawRectangle((int)ball.position.x, (int)ball.position.y, (int)ball.size.x, (int)ball.size.y, RED);
			DrawText(TextFormat("%i", player1.points), 0 + MeasureText("SCORE 00", 40) / 2, screenHeight / 2 - 20, 40, GRAY);
			DrawText(TextFormat("%i", player2.points), screenWidth - MeasureText("SCORE 00", 40) / 2 - 40, screenHeight / 2 - 20, 40, GRAY);

		EndDrawing();
    }
}

void MenuUpdate(void)
{
	if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
	{
		menuOption = 1;
	}

	if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
	{
		menuOption = 2;
	}

	if (IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_SPACE))
	{
		if (menuOption == 1)
		{
			isInMenu = false;
		}
		else
		{
			shouldExit = true;
		}
	}
}

void GameplayUpdate(void)
{
	// BALL MOVEMENT
	if (!gameOver)
	{
		ball.position.x += ball.speed.x * dt;
		ball.position.y += ball.speed.y * dt;
	}

	// PLAYER 1 CONTROLS
	if (IsKeyDown(KEY_W)) player1.position.y -= playerSpeed * dt;
	if (IsKeyDown(KEY_S)) player1.position.y += playerSpeed * dt;

	// PLAYER 2 CONTROLS
	if (IsKeyDown(KEY_UP)) player2.position.y -= playerSpeed * dt;
	if (IsKeyDown(KEY_DOWN)) player2.position.y += playerSpeed * dt;

	// COLLISIONS
	if (player1.position.y <= 0)
	{
		player1.position.y = 0;
	}

	if (player1.position.y + player1.size.y >= screenHeight)
	{
		player1.position.y = screenHeight - player1.size.y;
	}

	if (player2.position.y <= 0)
	{
		player2.position.y = 0;
	}
	
	if (player2.position.y + player2.size.y >= screenHeight)
	{
		player2.position.y = screenHeight - player2.size.y;
	}

	if (ball.position.y <= 0)
	{
		ball.position.y = 0;
		ball.speed.y *= -1;
	}

	if (ball.position.y + ball.size.y >= screenHeight)
	{
		ball.position.y = screenHeight - ball.size.y;
		ball.speed.y *= -1;
	}

	if (ball.position.x + ball.size.x >= screenWidth)
	{
		player1.points++;
		if (player1.points == 10) gameOver = true;
		ball.position = (Vector2) { screenWidth / 2 - ball.size.x / 2, screenHeight / 2 - ball.size.y / 2 };

		ball.speed.x = (float)ballSpeed / 2;
		ball.speed.y = 0;
	}

	if (ball.position.x <= 0)
	{
		player2.points++;
		if (player2.points == 10) gameOver = true;

		ball.position = (Vector2) { screenWidth / 2 - ball.size.x / 2, screenHeight / 2 - ball.size.y / 2 };

		ball.speed.x = (float)-ballSpeed / 2;
		ball.speed.y = 0;
	}      

	if (PlayerCollideWithBall(&player1, &ball))
	{
		ball.speed.x = ballSpeed;
		float factor = (player1.position.y + player1.size.y / 2) - (ball.position.y + ball.size.y / 2);
		ball.speed.y = factor * -10.0f;
		printf("%f\n", -factor);
	}

	if (PlayerCollideWithBall(&player2, &ball))
	{
		ball.speed.x = -ballSpeed;
		float factor = (player2.position.y + player2.size.y / 2) - (ball.position.y + ball.size.y / 2);
		ball.speed.y = factor * -10.0f;
		printf("%f\n", -factor);
	}
}

bool CoinFlip(void)
{
    srand((unsigned int)time(0));
    int flip = rand();
    if(flip % 2 == 0) return true;
    return false;
}

bool PlayerCollideWithBall(Player *p, Ball *b)
{
    if (p == NULL || b == NULL)
    {
        return false;
    }

	return (b->position.x >= p->position.x && b->position.x <= p->position.x + p->size.x) &&
	       (b->position.y <= p->position.y + p->size.y && b->position.y + ball.size.y >= p->position.y) ||
		   (b->position.x + b->size.x >= p->position.x && b->position.x <= p->position.x + p->size.x) &&
		   (b->position.y <= p->position.y + p->size.y && b->position.y + ball.size.y >= p->position.y);
}

