#include "level.h"
#include "player.h"
#include "raylib.h"
#include <cmath>
#include <algorithm>

Level::Level() : texturesLoaded(false), completed(false), isEcoPath(true),
                 enemiesDefeated(0), currentLevelNum(1) {}
Level::~Level() {}

// ─── Helpers ──────────────────────────────────────────────────────────────────
static Enemy MakeEnemy(float x, float y, float w, float h,
                        float minX, float maxX, float spd,
                        bool right, EnemyType type) {
    Enemy e;
    e.rect        = {x, y, w, h};
    e.patrolMinX  = minX; e.patrolMaxX = maxX;
    e.speed       = spd;  e.movingRight = right;
    e.active      = true; e.type = type;
    e.dying       = false; e.dyingTimer = 0;
    e.isChasing   = false;
    return e;
}

// ─── Textures ─────────────────────────────────────────────────────────────────
void Level::LoadTextures(bool ecoPath) {
    if (texturesLoaded) return;
    const char* bg = "assets/bg_level1.png";
    if (currentLevelNum == 1) bg = ecoPath ? "assets/bg_tariquia.png" : "assets/bg_industrial.png";
    else                      bg = ecoPath ? "assets/bg_level2.png"   : "assets/bg_industrial.png";
    bgTexture          = ::LoadTexture(bg);
    platformTexture    = ::LoadTexture("assets/platform.png");
    collectibleTexture = ::LoadTexture("assets/collectibles.png");
    obstacleTexture    = ::LoadTexture("assets/obstacles.png");
    enemyBlobTex       = ::LoadTexture("assets/enemy_blob.png");
    enemyComunarioTex  = ::LoadTexture("assets/enemy_comunario.png");
    texturesLoaded = true;
}

void Level::UnloadTextures() {
    if (!texturesLoaded) return;
    ::UnloadTexture(bgTexture); ::UnloadTexture(platformTexture);
    ::UnloadTexture(collectibleTexture); ::UnloadTexture(obstacleTexture);
    ::UnloadTexture(enemyBlobTex); ::UnloadTexture(enemyComunarioTex);
    texturesLoaded = false;
}

// ─── Level Load ───────────────────────────────────────────────────────────────
void Level::Load(int levelNum, bool ecoPath) {
    platforms.clear(); collectibles.clear();
    obstacles.clear(); enemies.clear();
    completed = false; isEcoPath = ecoPath;
    currentLevelNum = levelNum; enemiesDefeated = 0;
    goalRect = { LEVEL_WIDTH - 80, 255, 60, 80 };
    if (levelNum == 1) { if (ecoPath) BuildEcoLevel1(); else BuildEconLevel1(); }
    else               { if (ecoPath) BuildEcoLevel2(); else BuildEconLevel2(); }
}

// ── ECO L1: Bosque de Tariquía ────────────────────────────────────────────────
void Level::BuildEcoLevel1() {
    goalRect = {LEVEL_WIDTH-80, 255, 60, 80};
    platforms.push_back({0,405,450,45});
    platforms.push_back({530,405,600,45});
    platforms.push_back({1230,405,370,45});
    platforms.push_back({120,320,100,18}); platforms.push_back({290,245,90,18});
    platforms.push_back({450,300,80,18});  platforms.push_back({600,210,110,18});
    platforms.push_back({760,140,100,18}); platforms.push_back({900,210,90,18});
    platforms.push_back({680,320,80,18});  platforms.push_back({820,320,80,18});
    platforms.push_back({1050,345,100,18});platforms.push_back({1190,270,100,18});
    platforms.push_back({1340,200,100,18});platforms.push_back({1480,270,110,18});
    platforms.push_back({LEVEL_WIDTH-100,330,120,18});

    collectibles.push_back({{140,285,22,22},true,false});
    collectibles.push_back({{310,210,22,22},true,false});
    collectibles.push_back({{620,175,22,22},true,false});
    collectibles.push_back({{780,105,22,22},true,false});
    collectibles.push_back({{920,175,22,22},true,false});
    collectibles.push_back({{1210,235,22,22},true,false});
    collectibles.push_back({{1360,165,22,22},true,false});
    collectibles.push_back({{1500,235,22,22},true,false});

    obstacles.push_back({{390,375,38,30},true});
    obstacles.push_back({{700,375,38,30},true});
    obstacles.push_back({{1010,375,38,30},true});

    enemies.push_back(MakeEnemy(130,280,38,38,130,200,70,false,ENEMY_BLOB));
    enemies.push_back(MakeEnemy(610,172,38,38,600,690,80,true,ENEMY_BLOB));
    enemies.push_back(MakeEnemy(770,102,38,38,760,850,90,false,ENEMY_BLOB));
    enemies.push_back(MakeEnemy(905,172,38,38,895,975,75,true,ENEMY_BLOB));
    enemies.push_back(MakeEnemy(1200,232,38,38,1190,1270,85,false,ENEMY_BLOB));
    enemies.push_back(MakeEnemy(1350,162,38,38,1340,1440,80,true,ENEMY_BLOB));
}

// ── ECON L1: Zona de Perforación ─────────────────────────────────────────────
void Level::BuildEconLevel1() {
    goalRect = {LEVEL_WIDTH-80,255,60,80};
    platforms.push_back({0,405,600,45}); platforms.push_back({700,405,400,45});
    platforms.push_back({1200,405,400,45});
    platforms.push_back({100,330,120,18}); platforms.push_back({300,260,100,18});
    platforms.push_back({500,330,80,18});  platforms.push_back({650,200,120,18});
    platforms.push_back({830,140,100,18}); platforms.push_back({990,200,100,18});
    platforms.push_back({750,315,80,18});  platforms.push_back({1100,330,100,18});
    platforms.push_back({1260,260,100,18});platforms.push_back({1410,190,100,18});
    platforms.push_back({LEVEL_WIDTH-100,270,120,18});

    // Coins (money) collectibles on econ path
    collectibles.push_back({{120,295,22,22},true,false});
    collectibles.push_back({{320,225,22,22},true,false});
    collectibles.push_back({{670,165,22,22},true,false});
    collectibles.push_back({{850,105,22,22},true,false});
    collectibles.push_back({{1010,165,22,22},true,false});
    collectibles.push_back({{1280,225,22,22},true,false});
    collectibles.push_back({{1430,155,22,22},true,false});

    obstacles.push_back({{450,370,50,35},true}); obstacles.push_back({{700,370,50,35},true});
    obstacles.push_back({{1150,370,50,35},true});obstacles.push_back({{1380,370,50,35},true});

    // Comunarios: indigenous defenders that chase when close - shoot to defeat
    enemies.push_back(MakeEnemy(110,286,38,44,100,200,75,false,ENEMY_COMUNARIO));
    enemies.push_back(MakeEnemy(310,216,38,44,300,380,80,true,ENEMY_COMUNARIO));
    enemies.push_back(MakeEnemy(660,156,38,44,650,750,85,false,ENEMY_COMUNARIO));
    enemies.push_back(MakeEnemy(840,96,38,44,830,910,90,true,ENEMY_COMUNARIO));
    enemies.push_back(MakeEnemy(1000,156,38,44,990,1070,85,false,ENEMY_COMUNARIO));
    enemies.push_back(MakeEnemy(1270,216,38,44,1260,1340,90,true,ENEMY_COMUNARIO));
    enemies.push_back(MakeEnemy(1420,146,38,44,1410,LEVEL_WIDTH-100,95,false,ENEMY_COMUNARIO));
}

// ── ECO L2: Río San Telmo ─────────────────────────────────────────────────────
void Level::BuildEcoLevel2() {
    goalRect = {LEVEL_WIDTH-80,155,60,80};
    platforms.push_back({0,405,250,45});  platforms.push_back({350,405,200,45});
    platforms.push_back({700,405,250,45});platforms.push_back({1100,405,200,45});
    platforms.push_back({1400,405,200,45});
    platforms.push_back({80,330,90,18});  platforms.push_back({230,260,80,18});
    platforms.push_back({380,200,90,18}); platforms.push_back({520,130,90,18});
    platforms.push_back({680,200,80,18}); platforms.push_back({810,130,90,18});
    platforms.push_back({950,200,80,18}); platforms.push_back({700,315,70,18});
    platforms.push_back({1050,315,80,18});platforms.push_back({1150,245,90,18});
    platforms.push_back({1295,175,90,18});platforms.push_back({1435,230,100,18});
    platforms.push_back({LEVEL_WIDTH-100,200,120,18});

    collectibles.push_back({{100,295,22,22},true,true}); collectibles.push_back({{250,225,22,22},true,true});
    collectibles.push_back({{400,165,22,22},true,true}); collectibles.push_back({{540,95,22,22},true,true});
    collectibles.push_back({{830,95,22,22},true,true});  collectibles.push_back({{1170,210,22,22},true,true});
    collectibles.push_back({{1315,140,22,22},true,true});collectibles.push_back({{1455,195,22,22},true,true});

    obstacles.push_back({{310,370,40,35},true}); obstacles.push_back({{660,370,40,35},true});
    obstacles.push_back({{1060,370,40,35},true});

    enemies.push_back(MakeEnemy(90,290,40,40,80,160,90,false,ENEMY_BLOB));
    enemies.push_back(MakeEnemy(240,220,40,40,230,300,100,true,ENEMY_BLOB));
    enemies.push_back(MakeEnemy(390,160,40,40,380,460,110,false,ENEMY_BLOB));
    enemies.push_back(MakeEnemy(530,90,40,40,520,600,105,true,ENEMY_BLOB));
    enemies.push_back(MakeEnemy(820,90,40,40,810,890,115,false,ENEMY_BLOB));
    enemies.push_back(MakeEnemy(955,160,40,40,940,1020,100,true,ENEMY_BLOB));
    enemies.push_back(MakeEnemy(1165,205,40,40,1150,1230,110,false,ENEMY_BLOB));
    enemies.push_back(MakeEnemy(1305,130,40,40,1290,1380,120,true,ENEMY_BLOB));
}

// ── ECON L2: Tarija Industrial ────────────────────────────────────────────────
void Level::BuildEconLevel2() {
    goalRect = {LEVEL_WIDTH-80,155,60,80};
    platforms.push_back({0,405,300,45});  platforms.push_back({400,405,250,45});
    platforms.push_back({800,405,220,45});platforms.push_back({1150,405,300,45});
    platforms.push_back({1510,405,90,45});
    platforms.push_back({50,330,110,18}); platforms.push_back({220,260,90,18});
    platforms.push_back({370,190,100,18});platforms.push_back({510,120,90,18});
    platforms.push_back({660,195,90,18}); platforms.push_back({790,120,100,18});
    platforms.push_back({920,205,90,18}); platforms.push_back({660,310,80,18});
    platforms.push_back({1030,315,80,18});platforms.push_back({1150,245,90,18});
    platforms.push_back({1295,175,90,18});platforms.push_back({1440,235,90,18});
    platforms.push_back({LEVEL_WIDTH-100,205,120,18});

    collectibles.push_back({{70,295,22,22},true,false}); collectibles.push_back({{240,225,22,22},true,false});
    collectibles.push_back({{390,155,22,22},true,false});collectibles.push_back({{530,85,22,22},true,false});
    collectibles.push_back({{800,85,22,22},true,false}); collectibles.push_back({{1170,210,22,22},true,false});
    collectibles.push_back({{1315,140,22,22},true,false});collectibles.push_back({{1460,200,22,22},true,false});

    obstacles.push_back({{350,370,50,35},true}); obstacles.push_back({{700,370,50,35},true});
    obstacles.push_back({{1070,370,50,35},true});obstacles.push_back({{1390,370,50,35},true});

    enemies.push_back(MakeEnemy(60,286,38,44,50,150,95,false,ENEMY_COMUNARIO));
    enemies.push_back(MakeEnemy(230,216,38,44,220,310,100,true,ENEMY_COMUNARIO));
    enemies.push_back(MakeEnemy(380,146,38,44,370,460,110,false,ENEMY_COMUNARIO));
    enemies.push_back(MakeEnemy(520,76,38,44,510,590,115,true,ENEMY_COMUNARIO));
    enemies.push_back(MakeEnemy(670,151,38,44,660,740,120,false,ENEMY_COMUNARIO));
    enemies.push_back(MakeEnemy(800,76,38,44,790,870,120,true,ENEMY_COMUNARIO));
    enemies.push_back(MakeEnemy(930,161,38,44,920,1000,110,false,ENEMY_COMUNARIO));
    enemies.push_back(MakeEnemy(1160,201,38,44,1150,1230,115,true,ENEMY_COMUNARIO));
    enemies.push_back(MakeEnemy(1305,131,38,44,1295,1380,120,false,ENEMY_COMUNARIO));
}

// ─── Update ───────────────────────────────────────────────────────────────────
void Level::Update(Player& player, float& cameraX) {
    float dt = GetFrameTime();
    // Smooth camera follow
    float targetX = player.GetPosition().x - 400;
    if (targetX < 0) targetX = 0;
    if (targetX > LEVEL_WIDTH - 800) targetX = LEVEL_WIDTH - 800;
    cameraX += (targetX - cameraX) * 8.0f * dt;

    UpdateEnemies(dt, player.GetPosition().x, player.GetPosition().y);
    CheckEnemyPlayerCollision(player);
    CheckBulletEnemyCollision(player);

    Rectangle pRect = player.GetBounds();
    for (auto& col : collectibles) {
        if (col.active && CheckCollisionRecs(pRect, col.rect)) {
            col.active = false; player.AddScore(1);
        }
    }
    for (const auto& obs : obstacles) {
        if (CheckCollisionRecs(pRect, obs.rect)) player.TakeDamage();
    }
    if (CheckCollisionRecs(pRect, goalRect)) completed = true;
}

void Level::UpdateEnemies(float dt, float playerX, float playerY) {
    for (auto& e : enemies) {
        if (!e.active) continue;
        if (e.dying) {
            e.dyingTimer -= dt;
            if (e.dyingTimer <= 0) e.active = false;
            continue;
        }
        // Comunarios chase the player when within range
        if (e.type == ENEMY_COMUNARIO) {
            float dx = playerX - e.rect.x;
            float chaseRange = 220.0f;
            if (fabsf(dx) < chaseRange) {
                e.isChasing = true;
                float dir = (dx > 0) ? 1.0f : -1.0f;
                e.movingRight = (dir > 0);
                e.rect.x += dir * e.speed * 1.4f * dt;
            } else {
                e.isChasing = false;
                // Normal patrol
                if (e.movingRight) {
                    e.rect.x += e.speed * dt;
                    if (e.rect.x >= e.patrolMaxX) { e.rect.x = e.patrolMaxX; e.movingRight = false; }
                } else {
                    e.rect.x -= e.speed * dt;
                    if (e.rect.x <= e.patrolMinX) { e.rect.x = e.patrolMinX; e.movingRight = true; }
                }
            }
        } else {
            // ENEMY_BLOB: simple patrol
            if (e.movingRight) {
                e.rect.x += e.speed * dt;
                if (e.rect.x >= e.patrolMaxX) { e.rect.x = e.patrolMaxX; e.movingRight = false; }
            } else {
                e.rect.x -= e.speed * dt;
                if (e.rect.x <= e.patrolMinX) { e.rect.x = e.patrolMinX; e.movingRight = true; }
            }
        }
    }
}

void Level::CheckEnemyPlayerCollision(Player& player) {
    Rectangle pRect = player.GetBounds();
    Vector2 pVel = player.GetVelocity();

    for (auto& e : enemies) {
        if (!e.active || e.dying) continue;
        if (!CheckCollisionRecs(pRect, e.rect)) continue;

        if (e.type == ENEMY_BLOB) {
            // Blobs die from stomping
            float playerBottom = pRect.y + pRect.height;
            float enemyTop     = e.rect.y;
            bool stomping = (pVel.y > 0) && (playerBottom - enemyTop < 20);
            if (stomping) {
                e.dying = true; e.dyingTimer = 0.4f;
                enemiesDefeated++; player.StompBounce(); player.AddScore(2);
            } else {
                player.TakeDamage();
            }
        } else {
            // Comunarios: ONLY bullets can kill them, contact damages player
            player.TakeDamage();
        }
    }
}

void Level::CheckBulletEnemyCollision(Player& player) {
    auto& bullets = player.GetBullets();
    for (auto& b : bullets) {
        if (!b.active) continue;
        Rectangle bRect = {b.x, b.y, Bullet::W, Bullet::H};
        for (auto& e : enemies) {
            if (!e.active || e.dying) continue;
            if (CheckCollisionRecs(bRect, e.rect)) {
                b.active = false;
                e.dying = true;
                e.dyingTimer = 0.5f;
                enemiesDefeated++;
                player.AddScore(3); // bullets give more points
            }
        }
    }
}

int Level::GetEnemiesDefeated() const { return enemiesDefeated; }

// ─── Draw ─────────────────────────────────────────────────────────────────────
void Level::Draw(float cameraX) {
    if (texturesLoaded && bgTexture.id != 0) {
        DrawTexturePro(bgTexture,
            {0,0,(float)bgTexture.width,(float)bgTexture.height},
            {0,0,800,450},{0,0},0,WHITE);
    } else {
        ClearBackground(isEcoPath ? SKYBLUE : DARKGRAY);
    }

    for (const auto& plat : platforms) DrawPlatform(plat, cameraX);

    for (const auto& col : collectibles) {
        if (!col.active) continue;
        Rectangle sr = {col.rect.x - cameraX, col.rect.y, col.rect.width, col.rect.height};
        if (sr.x + sr.width < 0 || sr.x > 800) continue;
        if (texturesLoaded && collectibleTexture.id != 0) {
            float iW = (float)collectibleTexture.width / 3.0f;
            DrawTexturePro(collectibleTexture, {col.isWater ? iW : 0, 0, iW, (float)collectibleTexture.height},
                           sr, {0,0}, 0, WHITE);
        } else {
            DrawRectangleRec(sr, col.isWater ? BLUE : GREEN);
        }
    }

    for (const auto& obs : obstacles) {
        Rectangle sr = {obs.rect.x - cameraX, obs.rect.y, obs.rect.width, obs.rect.height};
        if (sr.x + sr.width < 0 || sr.x > 800) continue;
        if (texturesLoaded && obstacleTexture.id != 0) {
            float iW = (float)obstacleTexture.width / 2.0f;
            DrawTexturePro(obstacleTexture, {obs.isFire ? iW : 0, 0, iW, (float)obstacleTexture.height},
                           sr, {0,0}, 0, WHITE);
        } else {
            DrawRectangleRec(sr, obs.isFire ? RED : GRAY);
        }
    }

    // Goal with pulse
    Rectangle sg = {goalRect.x - cameraX, goalRect.y, goalRect.width, goalRect.height};
    DrawRectangleRec(sg, GOLD);
    DrawText("META", (int)sg.x+5, (int)sg.y-20, 16, GOLD);
    float pulse = (float)sin(GetTime()*4)*4;
    DrawText(">>>", (int)sg.x-38, (int)sg.y+20+(int)pulse, 18, YELLOW);

    DrawEnemies(cameraX);
}

void Level::DrawPlatform(Rectangle plat, float cameraX) {
    Rectangle sp = {plat.x - cameraX, plat.y, plat.width, plat.height};
    if (sp.x + sp.width < 0 || sp.x > 800) return;
    if (texturesLoaded && platformTexture.id != 0) {
        float tW = (float)platformTexture.width;
        float tH = (float)platformTexture.height;
        int tiles = (int)(plat.width / tW) + 1;
        for (int i = 0; i < tiles; i++) {
            float dx = sp.x + i * tW, dw = tW, sw = tW;
            if (dx + dw > sp.x + sp.width) { dw = (sp.x + sp.width) - dx; sw = dw; }
            if (dx + dw < 0 || dx > 800) continue;
            DrawTexturePro(platformTexture, {0,0,sw,tH}, {dx, sp.y, dw, sp.height}, {0,0}, 0, WHITE);
        }
    } else {
        DrawRectangleRec(sp, isEcoPath ? DARKBROWN : DARKGRAY);
        DrawRectangle((int)sp.x,(int)sp.y,(int)sp.width,6, isEcoPath ? DARKGREEN : GRAY);
    }
}

void Level::DrawEnemies(float cameraX) {
    for (const auto& e : enemies) {
        if (!e.active) continue;
        Rectangle sr = {e.rect.x - cameraX, e.rect.y, e.rect.width, e.rect.height};
        if (sr.x + sr.width < 0 || sr.x > 800) continue;

        float alpha = e.dying ? (e.dyingTimer > 0 ? e.dyingTimer / 0.5f : 0.1f) : 1.0f;
        Color tint = Fade(WHITE, alpha);

        // Draw enemy hit flash when dying
        if (e.dying) {
            DrawRectangle((int)sr.x-2,(int)sr.y-2,(int)sr.width+4,(int)sr.height+4,Fade(ORANGE,alpha));
        }

        // Chase indicator for comunarios (red tint when chasing)
        if (e.type == ENEMY_COMUNARIO && e.isChasing && !e.dying) {
            DrawRectangle((int)sr.x-2,(int)sr.y-2,(int)sr.width+4,(int)sr.height+4,{255,0,0,60});
        }

        Texture2D& tex = (e.type == ENEMY_BLOB) ? enemyBlobTex : enemyComunarioTex;
        if (texturesLoaded && tex.id != 0) {
            Rectangle src = {0,0,(float)tex.width,(float)tex.height};
            if (e.movingRight) src.width = -src.width;
            DrawTexturePro(tex, src, sr, {0,0}, 0, tint);
        } else {
            DrawRectangleRec(sr, e.type==ENEMY_BLOB ? Fade(GREEN,alpha) : Fade(ORANGE,alpha));
        }

        // Health-like indicator above enemy
        if (!e.dying) {
            Color barColor = (e.type == ENEMY_COMUNARIO) ? RED : Fade(DARKGREEN,0.8f);
            DrawRectangle((int)sr.x,(int)sr.y-8,(int)sr.width,5,{0,0,0,160});
            DrawRectangle((int)sr.x,(int)sr.y-8,(int)sr.width,5,barColor);
            if (e.type == ENEMY_COMUNARIO) {
                DrawText("! DISPARA !", (int)sr.x-10, (int)sr.y-20, 10, RED);
            }
        }
    }
}

bool Level::IsCompleted() const { return completed; }
const std::vector<Rectangle>& Level::GetPlatforms() const { return platforms; }
