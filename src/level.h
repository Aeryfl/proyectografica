#ifndef LEVEL_H
#define LEVEL_H

#include "raylib.h"
#include "enemy.h"
#include <vector>

#define LEVEL_WIDTH 1600.0f

struct Collectible {
    Rectangle rect;
    bool active;
    bool isWater;
};

struct Obstacle {
    Rectangle rect;
    bool isFire;
};

class Player;

class Level {
public:
    Level();
    ~Level();

    void Load(int levelNum, bool ecoPath);
    void LoadTextures(bool ecoPath);
    void UnloadTextures();

    void Update(Player& player, float& cameraX);
    void Draw(float cameraX);

    bool IsCompleted() const;
    const std::vector<Rectangle>& GetPlatforms() const;
    int GetEnemiesDefeated() const;

private:
    std::vector<Rectangle> platforms;
    std::vector<Collectible> collectibles;
    std::vector<Obstacle> obstacles;
    std::vector<Enemy> enemies;
    Rectangle goalRect;
    bool completed;
    bool isEcoPath;
    int enemiesDefeated;
    int currentLevelNum;

    Texture2D bgTexture;
    Texture2D platformTexture;
    Texture2D collectibleTexture;
    Texture2D obstacleTexture;
    Texture2D enemyBlobTex;
    Texture2D enemyDrillgolemTex;
    bool texturesLoaded;

    void BuildEcoLevel1();
    void BuildEcoLevel2();
    void BuildEconLevel1();
    void BuildEconLevel2();

    void UpdateEnemies(float dt, float playerX, float playerY);
    void CheckEnemyPlayerCollision(Player& player);
    void CheckBulletEnemyCollision(Player& player);
    void DrawEnemies(float cameraX);
    void DrawPlatform(Rectangle plat, float cameraX);
};

#endif // LEVEL_H
