#include "raylib.h"

int main(void)
{
    InitWindow(1920, 1080, "Block Kuzushi");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        // Title
        int titleSize = 90;
        int titleWidth = MeasureText("Block Kuzushi", titleSize);
        DrawText("Block Kuzushi", (1920 - titleWidth) / 2, 400, titleSize, WHITE);

        // Credit
        int creditSize = 20;
        int creditWidth = MeasureText("Mathias Novakovic", creditSize);
        DrawText("Credit: Mathias Novakovic", (1920 - creditWidth) / 2, 1000, creditSize, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
