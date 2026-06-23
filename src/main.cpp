#include "raylib.h"
#include "game.h"

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Guardianes de Tarija");
    InitAudioDevice();

    Game game;
    game.Init();

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        game.Update();

        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);
            game.Draw();
        EndDrawing();
    }

    // De-Initialization
    game.Unload();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
