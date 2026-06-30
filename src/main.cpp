#include "raylib.h"
#include "game.h"

int main(void)
{
    // Initialization
    // We want the window to be bigger (~800x600, preserving aspect ratio -> 1066x600)
    // but the internal game logic stays exactly at 800x450.
    const int windowWidth = 1066;
    const int windowHeight = 600;
    
    const int gameWidth = 800;
    const int gameHeight = 450;

    InitWindow(windowWidth, windowHeight, "Guardianes de Tarija");
    InitAudioDevice();

    RenderTexture2D target = LoadRenderTexture(gameWidth, gameHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT); // Keep pixels sharp when scaling

    Game game;
    game.Init();

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update game logic (always runs as if screen is 800x450)
        game.Update();

        // Draw into the internal 800x450 texture
        BeginTextureMode(target);
            ClearBackground(RAYWHITE);
            game.Draw();
        EndTextureMode();

        // Draw the texture scaled to the window size
        BeginDrawing();
            ClearBackground(BLACK);
            // DrawTexturePro parameters: texture, source rect, dest rect, origin, rotation, tint
            // Note: OpenGL textures are inverted vertically, so source height must be negative
            DrawTexturePro(target.texture, 
                { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                { 0.0f, 0.0f, (float)windowWidth, (float)windowHeight },
                { 0.0f, 0.0f }, 0.0f, WHITE);
        EndDrawing();
    }

    // De-Initialization
    UnloadRenderTexture(target);
    game.Unload();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
