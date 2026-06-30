#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "level.h"
#include "visual_novel.h"
#include "raylib.h"

enum GameState {
    MENU = 0,
    VN_INTRO,
    LEVEL_1,
    VN_MIDDLE,
    LEVEL_2,
    VN_ENDING,
    GAME_OVER
};

class Game {
public:
    Game();
    ~Game();

    void Init();
    void Update();
    void Draw();
    void Unload();

private:
    GameState currentState;
    Player player;
    Level currentLevel;
    VisualNovel visualNovel;
    Camera2D camera;

    int karma;
    bool ecoPath; // true = protect Tariquía, false = economic/drilling path
    float cameraX;

    Texture2D menuBgTexture;
    bool menuTextureLoaded;
    
    Music bgMusic;
    bool musicLoaded;

    void LoadLevel(int num);
    void UpdateCamera();
    void DrawMenu();
    void DrawGameHUD();
    void DrawGameOver();
    void ResetGame();
};

#endif // GAME_H
