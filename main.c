#include "raylib.h"


typedef enum GameScreen { MENU, GAME, INSTRUCTIONS, EXIT, GAME_WIN } GameScreen;

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

int score = 0;
int highScore = 0;

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

typedef struct PowerUp {
    Rectangle rect;
    float fallSpeed;
    bool active;
} PowerUp;


void InitGame(Paddle *paddle, Ball *ball, Brick bricks[BRICK_ROWS][BRICK_COLUMNS], PowerUp *powerUp) {
    score = 0;
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

    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int col = 0; col < BRICK_COLUMNS; col++) {
            bricks[row][col].rect = (Rectangle){
                col * (BRICK_WIDTH + BRICK_SPACING) + 100,
                row * (BRICK_HEIGHT + BRICK_SPACING) + 100,
                BRICK_WIDTH, BRICK_HEIGHT
            };
            bricks[row][col].active = true;
            bricks[row][col].color = rainbowColors[row % 5];
        }
    }

    powerUp->active = false;
}


void UpdateGame(Paddle *paddle, Ball *ball, Brick bricks[BRICK_ROWS][BRICK_COLUMNS], GameScreen *screen, PowerUp *powerUp) {
    if (IsKeyPressed(KEY_ESCAPE)) *screen = MENU;

    if (IsKeyDown(KEY_LEFT) && paddle->rect.x > 0)
        paddle->rect.x -= paddle->speed * GetFrameTime();

    if (IsKeyDown(KEY_RIGHT) && paddle->rect.x < SCREEN_WIDTH - PADDLE_WIDTH)
        paddle->rect.x += paddle->speed * GetFrameTime();

    if (ball->active) {
        ball->position.x += ball->speed.x;
        ball->position.y += ball->speed.y;

        if (ball->position.x - ball->radius <= 0 || ball->position.x + ball->radius >= SCREEN_WIDTH)
            ball->speed.x *= -1;

        if (ball->position.y - ball->radius <= 0)
            ball->speed.y *= -1;

        if (CheckCollisionCircleRec(ball->position, ball->radius, paddle->rect)) {
            ball->speed.y *= -1;
            ball->position.y = paddle->rect.y - ball->radius;
        }


        for (int row = 0; row < BRICK_ROWS; row++) {
            for (int col = 0; col < BRICK_COLUMNS; col++) {
                if (bricks[row][col].active && CheckCollisionCircleRec(ball->position, ball->radius, bricks[row][col].rect)) {
                    bricks[row][col].active = false;
                    ball->speed.y *= -1;
                    score += 100;
                    if (score > highScore) highScore = score;


                    if (!powerUp->active && GetRandomValue(0, 100) < 20) {
                        powerUp->active = true;
                        powerUp->rect.width = 30;
                        powerUp->rect.height = 30;

                        powerUp->rect.x = bricks[row][col].rect.x + bricks[row][col].rect.width / 2 - powerUp->rect.width / 2;
                        powerUp->rect.y = bricks[row][col].rect.y + bricks[row][col].rect.height / 2 - powerUp->rect.height / 2;
                        powerUp->fallSpeed = 300.0f;
                    }
                }
            }
        }


        if (powerUp->active) {
            powerUp->rect.y += powerUp->fallSpeed * GetFrameTime();

            if (CheckCollisionRecs(paddle->rect, powerUp->rect)) {
                paddle->speed += 200.0f;
                powerUp->active = false;
            }

            if (powerUp->rect.y > SCREEN_HEIGHT) {
                powerUp->active = false;
            }
        }


        bool allBricksDestroyed = true;
        for (int row = 0; row < BRICK_ROWS; row++) {
            for (int col = 0; col < BRICK_COLUMNS; col++) {
                if (bricks[row][col].active) {
                    allBricksDestroyed = false;
                    break;
                }
            }
        }
        if (allBricksDestroyed) {
            *screen = GAME_WIN;
        }

        if (ball->position.y > SCREEN_HEIGHT) {
            ball->active = false;
            *screen = MENU;
        }
    }
}


void DrawGame(Paddle *paddle, Ball *ball, Brick bricks[BRICK_ROWS][BRICK_COLUMNS], PowerUp *powerUp) {
    DrawRectangleRec(paddle->rect, WHITE);

    if (ball->active)
        DrawCircleV(ball->position, ball->radius, ORANGE);

    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int col = 0; col < BRICK_COLUMNS; col++) {
            if (bricks[row][col].active)
                DrawRectangleRec(bricks[row][col].rect, bricks[row][col].color);
        }
    }


    if (powerUp->active) {
        DrawRectangleRec(powerUp->rect, YELLOW);
    }

    DrawText(TextFormat("Score: %d", score), 50, 50, 40, WHITE);
    DrawText(TextFormat("High Score: %d", highScore), 50, 100, 40, WHITE);
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Block Kuzushi");
    SetTargetFPS(60);

    GameScreen currentScreen = MENU;
    int menuSelection = 0;

    Paddle paddle;
    Ball ball;
    Brick bricks[BRICK_ROWS][BRICK_COLUMNS];
    PowerUp speedPowerUp;
    speedPowerUp.active = false;

    while (!WindowShouldClose()) {

        switch (currentScreen) {
            case MENU:

                if (IsKeyPressed(KEY_DOWN)) {
                    menuSelection++;
                    if (menuSelection > 2) menuSelection = 0;
                }
                if (IsKeyPressed(KEY_UP)) {
                    menuSelection--;
                    if (menuSelection < 0) menuSelection = 2;
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    switch (menuSelection) {
                        case 0:
                            InitGame(&paddle, &ball, bricks, &speedPowerUp);
                            currentScreen = GAME;
                            break;
                        case 1:
                            currentScreen = INSTRUCTIONS;
                            break;
                        case 2:
                            currentScreen = EXIT;
                            break;
                    }
                }
                break;
            case INSTRUCTIONS:
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    currentScreen = MENU;
                }
                break;
            case GAME:
                UpdateGame(&paddle, &ball, bricks, &currentScreen, &speedPowerUp);
                break;
            case GAME_WIN:
                if (IsKeyPressed(KEY_ENTER)) {
                    currentScreen = MENU;
                }
                break;
            case EXIT:
                CloseWindow();
                return 0;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        switch (currentScreen) {
            case MENU: {
                const char *menuTitle = "Block Kuzushi";
                DrawText(menuTitle, SCREEN_WIDTH / 2 - MeasureText(menuTitle, 60) / 2, SCREEN_HEIGHT / 2 - 150, 60, WHITE);

                const char *menuOptions[3] = { "Start Game", "Instructions", "Quit Game" };
                for (int i = 0; i < 3; i++) {
                    int textWidth = MeasureText(menuOptions[i], 40);
                    int posY = SCREEN_HEIGHT / 2 - 50 + i * 50;
                    if (i == menuSelection) {

                        DrawText(">", SCREEN_WIDTH / 2 - textWidth / 2 - 50, posY, 40, YELLOW);
                        DrawText(menuOptions[i], SCREEN_WIDTH / 2 - textWidth / 2 - 20, posY, 40, YELLOW);
                    } else {
                        DrawText(menuOptions[i], SCREEN_WIDTH / 2 - textWidth / 2, posY, 40, WHITE);
                    }
                }
                DrawText("Credits: Mathias Novakovic Forsberg Skola GP24",
                         SCREEN_WIDTH / 2 - MeasureText("Credits: Mathias Novakovic Forsberg GP24", 20) / 2,
                         SCREEN_HEIGHT - 50, 20, WHITE);
            } break;
            case INSTRUCTIONS:
                DrawText("Use LEFT and RIGHT arrow keys to move the paddle.", 100, 100, 30, WHITE);
                DrawText("Destroy bricks to score points.", 100, 150, 30, WHITE);
                DrawText("Catch the yellow power up to increase paddle speed.", 100, 200, 30, WHITE);
                DrawText("Press BACKSPACE to go back.", 100, 250, 30, WHITE);
                break;
            case GAME:
                DrawGame(&paddle, &ball, bricks, &speedPowerUp);
                break;
            case GAME_WIN:
                DrawText("You win! Press ENTER to return to menu.",
                         SCREEN_WIDTH / 2 - MeasureText("You win! Press ENTER to return to menu.", 40) / 2,
                         SCREEN_HEIGHT / 2, 40, WHITE);
                break;
            default:
                break;
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
