#include "game.h"
#include "raylib.h"

Game::Game() : currentState(MENU), karma(0), ecoPath(true),
               menuTextureLoaded(false), cameraX(0) {}
Game::~Game() {}

void Game::Init() {
    menuBgTexture = ::LoadTexture("assets/bg_menu.png");
    menuTextureLoaded = (menuBgTexture.id != 0);
    player.LoadTextures();
    visualNovel.LoadTextures();
}

void Game::ResetGame() {
    karma = 0; ecoPath = true; cameraX = 0;
    player.Init();
    currentLevel.UnloadTextures();
    currentState = MENU;
}

void Game::Update() {
    switch (currentState) {
        case MENU:
            if (IsKeyPressed(KEY_ENTER)) {
                currentState = VN_INTRO;
                visualNovel.LoadScene(0); // Intro: Tariquía crisis
            }
            break;

        case VN_INTRO:
            visualNovel.Update();
            if (visualNovel.IsSceneFinished()) {
                karma += visualNovel.GetKarmaDelta();
                ecoPath = (karma > 0);
                LoadLevel(1);
                currentState = LEVEL_1;
            }
            break;

        case LEVEL_1:
            player.Update(currentLevel);
            currentLevel.Update(player, cameraX);
            if (currentLevel.IsCompleted()) {
                // Load mid-VN based on path taken (1=eco, 2=econ)
                visualNovel.LoadScene(ecoPath ? 1 : 2);
                currentState = VN_MIDDLE;
            }
            if (!player.IsAlive()) {
                currentLevel.UnloadTextures();
                currentState = GAME_OVER;
            }
            break;

        case VN_MIDDLE:
            visualNovel.Update();
            if (visualNovel.IsSceneFinished()) {
                karma += visualNovel.GetKarmaDelta();
                ecoPath = (karma > 0);
                LoadLevel(2);
                currentState = LEVEL_2;
            }
            break;

        case LEVEL_2:
            player.Update(currentLevel);
            currentLevel.Update(player, cameraX);
            if (currentLevel.IsCompleted()) {
                currentLevel.UnloadTextures();
                visualNovel.LoadScene(karma > 0 ? 3 : 4); // 3=good, 4=bad
                currentState = VN_ENDING;
            }
            if (!player.IsAlive()) {
                currentLevel.UnloadTextures();
                currentState = GAME_OVER;
            }
            break;

        case VN_ENDING:
            visualNovel.Update();
            if (visualNovel.IsSceneFinished() && IsKeyPressed(KEY_ENTER)) {
                ResetGame();
            }
            break;

        case GAME_OVER:
            if (IsKeyPressed(KEY_ENTER)) ResetGame();
            break;
    }
}

void Game::LoadLevel(int num) {
    currentLevel.UnloadTextures();
    currentLevel.Load(num, ecoPath);
    currentLevel.LoadTextures(ecoPath);
    cameraX = 0;
    player.SetEcoMode(ecoPath); // shooting only on economic path
    player.ResetPosition(80, 300);
    if (num == 1) player.Init(); player.LoadTextures();
}

void Game::UpdateCamera() {} // camera is now managed inside level.Update

void Game::Draw() {
    switch (currentState) {
        case MENU: DrawMenu(); break;

        case VN_INTRO:
        case VN_MIDDLE:
        case VN_ENDING:
            visualNovel.Draw();
            break;

        case LEVEL_1:
        case LEVEL_2:
            currentLevel.Draw(cameraX);
            player.Draw(cameraX);
            DrawGameHUD();
            break;

        case GAME_OVER: DrawGameOver(); break;
    }
}

void Game::DrawGameHUD() {
    // HUD bar
    DrawRectangle(0, 0, 800, 40, {0,0,0,170});
    const char* levelName = (currentState == LEVEL_1)
        ? (ecoPath ? "NIVEL 1 - Bosque de Tariquía" : "NIVEL 1 - Zona de Perforación")
        : (ecoPath ? "NIVEL 2 - Río San Telmo"      : "NIVEL 2 - Tarija Industrial");
    DrawText(levelName, 10, 11, 18, ecoPath ? GREEN : ORANGE);
    DrawText(TextFormat("Puntos: %d", player.GetScore()), 580, 11, 18, GOLD);
    for (int i = 0; i < player.GetLives(); i++)
        DrawText("<3", 700 + i * 34, 11, 20, RED);
    // Mini progress bar
    float progress = (player.GetPosition().x) / LEVEL_WIDTH;
    DrawRectangle(200, 28, 380, 8, {80,80,80,180});
    DrawRectangle(200, 28, (int)(380 * progress), 8, ecoPath ? GREEN : ORANGE);
    DrawText("->META", 585, 26, 12, GOLD);
}

void Game::DrawMenu() {
    if (menuTextureLoaded && menuBgTexture.id != 0)
        DrawTexturePro(menuBgTexture, {0,0,(float)menuBgTexture.width,(float)menuBgTexture.height},
                       {0,0,800,450}, {0,0}, 0, WHITE);
    else ClearBackground(DARKGREEN);

    DrawRectangle(80, 90, 640, 155, {0,0,0,175});
    DrawText("GUARDIANES DE TARIJA", 95, 100, 44, GOLD);
    DrawText("La Reserva Natural de Tariquia necesita un heroe.", 120, 158, 19, RAYWHITE);
    DrawText("Tu decision cambiara el destino.", 205, 185, 18, LIGHTGRAY);

    if ((int)(GetTime()*2)%2==0)
        DrawText("Presiona ENTER para comenzar", 240, 310, 22, WHITE);

    DrawText("Flechas/WASD: Mover  |  Espacio/W: Saltar  |  Pisa enemigos para derrotarlos", 65, 425, 13, {180,180,180,200});
}

void Game::DrawGameOver() {
    ClearBackground({8,4,4,255});
    DrawText("PERDISTE", 275, 100, 55, RED);
    DrawText("Tariquía te necesitaba más...", 235, 185, 24, LIGHTGRAY);
    DrawText(TextFormat("Puntos obtenidos: %d", player.GetScore()), 295, 240, 22, GOLD);
    if ((int)(GetTime()*2)%2==0)
        DrawText("ENTER para volver al menú", 250, 330, 22, WHITE);
}

void Game::Unload() {
    if (menuTextureLoaded) { ::UnloadTexture(menuBgTexture); menuTextureLoaded = false; }
    player.UnloadTextures();
    visualNovel.UnloadTextures();
    currentLevel.UnloadTextures();
}
