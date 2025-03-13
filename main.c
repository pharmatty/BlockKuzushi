#include "raylib.h"

typedef enum GameScreen { MENU, GAME, INSTRUCTIONS, EXIT } GameScreen;

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define PADDLE_WIDTH 150
#define PADDLE_HEIGHT 20
#define BALL_RADIUS 10
#define BRICK_ROWS 5
#define BRICK_COLUMNS 10
#define BRICK_WIDTH 150
#define BRICK_HEIGHT 40
#define BRICK_SPACING 10

typedef struct Ball {
    Vector2 position;
    Vector2 speed;
    float radius;
    bool active;
} Ball;

typedef struct Paddle {
    Rectangle rect;
    float speed;
} Paddle;

typedef struct Brick {
    Rectangle rect;
    bool active;
    Color color;
} Brick;

void InitGame(Paddle *paddle, Ball *ball, Brick bricks[BRICK_ROWS][BRICK_COLUMNS]);
void UpdateGame(Paddle *paddle, Ball *ball, Brick bricks[BRICK_ROWS][BRICK_COLUMNS], GameScreen *screen);
void DrawGame(Paddle *paddle, Ball *ball, Brick bricks[BRICK_ROWS][BRICK_COLUMNS]);

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Block Kuzushi");
    SetTargetFPS(60);

    GameScreen currentScreen = MENU;
    int selectedOption = 0;

    Paddle paddle;
    Ball ball;
    Brick bricks[BRICK_ROWS][BRICK_COLUMNS];

    while (!WindowShouldClose())
    {
        if (currentScreen == MENU)
        {
            if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % 3;
            if (IsKeyPressed(KEY_UP)) selectedOption = (selectedOption - 1 + 3) % 3;

            if (IsKeyPressed(KEY_ENTER))
            {
                if (selectedOption == 0)
                {
                    InitGame(&paddle, &ball, bricks);
                    currentScreen = GAME;
                }
                else if (selectedOption == 1)
                {
                    currentScreen = INSTRUCTIONS;
                }
                else if (selectedOption == 2)
                {
                    currentScreen = EXIT;
                }
            }

            BeginDrawing();
            ClearBackground(BLACK);

            int titleSize = 90;
            int titleWidth = MeasureText("Block Kuzushi", titleSize);
            DrawText("Block Kuzushi", (SCREEN_WIDTH - titleWidth) / 2, 300, titleSize, WHITE);

            int optionSize = 50;
            Color startColor = selectedOption == 0 ? YELLOW : WHITE;
            Color instructionsColor = selectedOption == 1 ? YELLOW : WHITE;
            Color quitColor = selectedOption == 2 ? YELLOW : WHITE;

            DrawText("Start Game", (SCREEN_WIDTH - MeasureText("Start Game", optionSize)) / 2, 500, optionSize, startColor);
            DrawText("Instructions", (SCREEN_WIDTH - MeasureText("Instructions", optionSize)) / 2, 600, optionSize, instructionsColor);
            DrawText("Quit", (SCREEN_WIDTH - MeasureText("Quit", optionSize)) / 2, 700, optionSize, quitColor);

            int creditsSize = 30;
            int creditsWidth = MeasureText("Made by Mathias Novakovic | GP24 Forsbergs Skola", creditsSize);
            DrawText("Made by Mathias Novakovic | GP24 Forsbergs Skola",
                     (SCREEN_WIDTH - creditsWidth) / 2, SCREEN_HEIGHT - 50, creditsSize, GRAY);

            EndDrawing();
        }
        else if (currentScreen == INSTRUCTIONS)
        {
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER))
            {
                currentScreen = MENU;
            }

            BeginDrawing();
            ClearBackground(BLACK);

            int instructionSize = 40;
            DrawText("Instructions", (SCREEN_WIDTH - MeasureText("Instructions", 60)) / 2, 200, 60, WHITE);
            DrawText("Use Left/Right Arrow Keys to Move", 500, 400, instructionSize, WHITE);
            DrawText("Press ESC to return to Menu", 500, 500, instructionSize, WHITE);

            EndDrawing();
        }
        else if (currentScreen == GAME)
        {
            UpdateGame(&paddle, &ball, bricks, &currentScreen);

            BeginDrawing();
            ClearBackground(BLACK);
            DrawGame(&paddle, &ball, bricks);
            EndDrawing();
        }
        else if (currentScreen == EXIT)
        {
            break;
        }
    }

    CloseWindow();
    return 0;
}

void InitGame(Paddle *paddle, Ball *ball, Brick bricks[BRICK_ROWS][BRICK_COLUMNS])
{
    paddle->rect = (Rectangle){ SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2, SCREEN_HEIGHT - 100, PADDLE_WIDTH, PADDLE_HEIGHT };
    paddle->speed = 600.0f;

    ball->position = (Vector2){ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
    ball->speed = (Vector2){ 6, -6 };
    ball->radius = BALL_RADIUS;
    ball->active = true;

    Color rainbowColors[5] = {
        (Color){200, 80, 80, 255},
        (Color){200, 160, 80, 255},
        (Color){160, 200, 80, 255},
        (Color){80, 160, 200, 255},
        (Color){160, 80, 200, 255}
    };

    for (int row = 0; row < BRICK_ROWS; row++)
    {
        for (int col = 0; col < BRICK_COLUMNS; col++)
        {
            bricks[row][col].rect.x = col * (BRICK_WIDTH + BRICK_SPACING) + 100;
            bricks[row][col].rect.y = row * (BRICK_HEIGHT + BRICK_SPACING) + 100;
            bricks[row][col].rect.width = BRICK_WIDTH;
            bricks[row][col].rect.height = BRICK_HEIGHT;
            bricks[row][col].active = true;
            bricks[row][col].color = rainbowColors[row % 5];
        }
    }
}

void UpdateGame(Paddle *paddle, Ball *ball, Brick bricks[BRICK_ROWS][BRICK_COLUMNS], GameScreen *screen)
{
    if (IsKeyPressed(KEY_ESCAPE)) *screen = MENU;

    if (IsKeyDown(KEY_LEFT) && paddle->rect.x > 0)
        paddle->rect.x -= paddle->speed * GetFrameTime();

    if (IsKeyDown(KEY_RIGHT) && paddle->rect.x < SCREEN_WIDTH - PADDLE_WIDTH)
        paddle->rect.x += paddle->speed * GetFrameTime();

    if (ball->active)
    {
        ball->position.x += ball->speed.x;
        ball->position.y += ball->speed.y;

        if (ball->position.x - ball->radius <= 0 || ball->position.x + ball->radius >= SCREEN_WIDTH)
            ball->speed.x *= -1;

        if (ball->position.y - ball->radius <= 0)
            ball->speed.y *= -1;

        if (CheckCollisionCircleRec(ball->position, ball->radius, paddle->rect))
        {
            ball->speed.y *= -1;
            ball->position.y = paddle->rect.y - ball->radius;
        }

        for (int row = 0; row < BRICK_ROWS; row++)
        {
            for (int col = 0; col < BRICK_COLUMNS; col++)
            {
                if (bricks[row][col].active && CheckCollisionCircleRec(ball->position, ball->radius, bricks[row][col].rect))
                {
                    bricks[row][col].active = false;
                    ball->speed.y *= -1;
                }
            }
        }

        if (ball->position.y > SCREEN_HEIGHT)
        {
            ball->active = false;
            *screen = MENU;
        }
    }
}

void DrawGame(Paddle *paddle, Ball *ball, Brick bricks[BRICK_ROWS][BRICK_COLUMNS])
{
    DrawRectangleRec(paddle->rect, WHITE);

    if (ball->active)
        DrawCircleV(ball->position, ball->radius, ORANGE);

    for (int row = 0; row < BRICK_ROWS; row++)
    {
        for (int col = 0; col < BRICK_COLUMNS; col++)
        {
            if (bricks[row][col].active)
                DrawRectangleRec(bricks[row][col].rect, bricks[row][col].color);
        }
    }
}
