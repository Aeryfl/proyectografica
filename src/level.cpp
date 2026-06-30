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
    e.health      = (type == ENEMY_DRILLGOLEM) ? 2 : 1;
    e.stunnedTimer = 0.0f;
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
    enemyDrillgolemTex = ::LoadTexture("assets/enemy_drillgolem.png");
    texturesLoaded = true;
}

void Level::UnloadTextures() {
    if (!texturesLoaded) return;
    ::UnloadTexture(bgTexture); ::UnloadTexture(platformTexture);
    ::UnloadTexture(collectibleTexture); ::UnloadTexture(obstacleTexture);
    ::UnloadTexture(enemyBlobTex); ::UnloadTexture(enemyDrillgolemTex);
    texturesLoaded = false;
}

// ─── Level Load ───────────────────────────────────────────────────────────────
void Level::Load(int levelNum, bool ecoPath) {
    platforms.clear(); collectibles.clear();
    obstacles.clear(); enemies.clear();
    completed = false; isEcoPath = ecoPath;
    currentLevelNum = levelNum; enemiesDefeated = 0;
    goalRect = { LEVEL_WIDTH - 80, 355, 60, 80 }; // goal always on ground level
    if (levelNum == 1) { if (ecoPath) BuildEcoLevel1(); else BuildEconLevel1(); }
    else               { if (ecoPath) BuildEcoLevel2(); else BuildEconLevel2(); }
}

// ═══════════════════════════════════════════════════════════════════════════════
// LEVEL DESIGN RULES:
// - Continuous ground (y=405, height=45) spanning full LEVEL_WIDTH — NO pits
// - Optional floating platforms for collectibles — NOT required to complete level
// - Zero obstacles (no fire/barrels)
// - 2-3 slow enemies per level, simple patrol only
// - Goal at ground level (y=355) — always reachable by walking right
// ═══════════════════════════════════════════════════════════════════════════════

// ── ECO L1: Bosque de Tariquía ────────────────────────────────────────────────
void Level::BuildEcoLevel1() {
    goalRect = {LEVEL_WIDTH - 80, 355, 60, 80};

    // CONTINUOUS ground — no gaps, no falls possible
    platforms.push_back({0, 405, LEVEL_WIDTH, 45});

    // Optional floating platforms for bonus collectibles
    platforms.push_back({200,  330, 150, 18});
    platforms.push_back({500,  310, 150, 18});
    platforms.push_back({800,  330, 150, 18});
    platforms.push_back({1100, 310, 150, 18});
    platforms.push_back({1350, 330, 150, 18});

    // Collectibles — some on ground, some on platforms (all optional)
    collectibles.push_back({{250, 295, 22, 22}, true, false});
    collectibles.push_back({{550, 275, 22, 22}, true, false});
    collectibles.push_back({{850, 295, 22, 22}, true, false});
    collectibles.push_back({{1150,275, 22, 22}, true, false});
    collectibles.push_back({{1400,295, 22, 22}, true, false});

    // NO obstacles

    // 2 blobs — slow, wide patrol, on ground level
    enemies.push_back(MakeEnemy(400, 367, 38, 38, 350, 550, 50, true, ENEMY_BLOB));
    enemies.push_back(MakeEnemy(1000,367, 38, 38, 950, 1150, 50, false, ENEMY_BLOB));
}

// ── ECON L1: Zona de Perforación ─────────────────────────────────────────────
void Level::BuildEconLevel1() {
    goalRect = {LEVEL_WIDTH - 80, 355, 60, 80};

    // CONTINUOUS ground
    platforms.push_back({0, 405, LEVEL_WIDTH, 45});

    // Optional platforms
    platforms.push_back({200,  330, 150, 18});
    platforms.push_back({500,  310, 150, 18});
    platforms.push_back({800,  330, 150, 18});
    platforms.push_back({1100, 310, 150, 18});
    platforms.push_back({1350, 330, 150, 18});

    // Collectibles
    collectibles.push_back({{250, 295, 22, 22}, true, false});
    collectibles.push_back({{550, 275, 22, 22}, true, false});
    collectibles.push_back({{850, 295, 22, 22}, true, false});
    collectibles.push_back({{1150,275, 22, 22}, true, false});
    collectibles.push_back({{1400,295, 22, 22}, true, false});

    // NO obstacles

    // 2 Drill-Golems — slow patrol on ground
    enemies.push_back(MakeEnemy(400, 361, 40, 44, 350, 550, 45, true, ENEMY_DRILLGOLEM));
    enemies.push_back(MakeEnemy(1000,361, 40, 44, 950, 1150, 45, false, ENEMY_DRILLGOLEM));
}

// ── ECO L2: Río San Telmo ─────────────────────────────────────────────────────
void Level::BuildEcoLevel2() {
    goalRect = {LEVEL_WIDTH - 80, 355, 60, 80};

    // CONTINUOUS ground
    platforms.push_back({0, 405, LEVEL_WIDTH, 45});

    // More platforms (slightly more interesting)
    platforms.push_back({150,  330, 140, 18});
    platforms.push_back({380,  310, 140, 18});
    platforms.push_back({600,  280, 140, 18});
    platforms.push_back({850,  310, 140, 18});
    platforms.push_back({1100, 330, 140, 18});
    platforms.push_back({1350, 310, 140, 18});

    // Collectibles — water drops
    collectibles.push_back({{195, 295, 22, 22}, true, true});
    collectibles.push_back({{425, 275, 22, 22}, true, true});
    collectibles.push_back({{645, 245, 22, 22}, true, true});
    collectibles.push_back({{895, 275, 22, 22}, true, true});
    collectibles.push_back({{1145,295, 22, 22}, true, true});
    collectibles.push_back({{1395,275, 22, 22}, true, true});

    // NO obstacles

    // 3 blobs — a bit faster than L1
    enemies.push_back(MakeEnemy(300, 367, 38, 38, 250, 450, 55, true, ENEMY_BLOB));
    enemies.push_back(MakeEnemy(700, 367, 38, 38, 650, 850, 55, false, ENEMY_BLOB));
    enemies.push_back(MakeEnemy(1200,367, 38, 38, 1150, 1350, 55, true, ENEMY_BLOB));
}

// ── ECON L2: Tarija Industrial ────────────────────────────────────────────────
void Level::BuildEconLevel2() {
    goalRect = {LEVEL_WIDTH - 80, 355, 60, 80};

    // CONTINUOUS ground
    platforms.push_back({0, 405, LEVEL_WIDTH, 45});

    // Platforms
    platforms.push_back({150,  330, 140, 18});
    platforms.push_back({380,  310, 140, 18});
    platforms.push_back({600,  280, 140, 18});
    platforms.push_back({850,  310, 140, 18});
    platforms.push_back({1100, 330, 140, 18});
    platforms.push_back({1350, 310, 140, 18});

    // Collectibles
    collectibles.push_back({{195, 295, 22, 22}, true, false});
    collectibles.push_back({{425, 275, 22, 22}, true, false});
    collectibles.push_back({{645, 245, 22, 22}, true, false});
    collectibles.push_back({{895, 275, 22, 22}, true, false});
    collectibles.push_back({{1145,295, 22, 22}, true, false});
    collectibles.push_back({{1395,275, 22, 22}, true, false});

    // NO obstacles

    // 3 Drill-Golems — a bit faster
    enemies.push_back(MakeEnemy(300, 361, 40, 44, 250, 450, 50, true, ENEMY_DRILLGOLEM));
    enemies.push_back(MakeEnemy(700, 361, 40, 44, 650, 850, 50, false, ENEMY_DRILLGOLEM));
    enemies.push_back(MakeEnemy(1200,361, 40, 44, 1150, 1350, 50, true, ENEMY_DRILLGOLEM));
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
    // Obstacles removed — no damage from obstacles
    if (CheckCollisionRecs(pRect, goalRect)) completed = true;
}

// ALL enemies now use SIMPLE PATROL ONLY — no chasing behavior
void Level::UpdateEnemies(float dt, float playerX, float playerY) {
    for (auto& e : enemies) {
        if (!e.active) continue;
        if (e.dying) {
            e.dyingTimer -= dt;
            if (e.dyingTimer <= 0) e.active = false;
            continue;
        }
        // Tick stun timer
        if (e.stunnedTimer > 0) {
            e.stunnedTimer -= dt;
            if (e.stunnedTimer < 0) e.stunnedTimer = 0;
        }

        // Simple patrol for ALL enemy types — no chasing
        e.isChasing = false;
        if (e.movingRight) {
            e.rect.x += e.speed * dt;
            if (e.rect.x >= e.patrolMaxX) { e.rect.x = e.patrolMaxX; e.movingRight = false; }
        } else {
            e.rect.x -= e.speed * dt;
            if (e.rect.x <= e.patrolMinX) { e.rect.x = e.patrolMinX; e.movingRight = true; }
        }
    }
}

void Level::CheckEnemyPlayerCollision(Player& player) {
    Rectangle pRect = player.GetBounds();
    Vector2 pVel = player.GetVelocity();

    for (auto& e : enemies) {
        if (!e.active || e.dying) continue;
        if (!CheckCollisionRecs(pRect, e.rect)) continue;

        float playerBottom = pRect.y + pRect.height;
        float enemyTop     = e.rect.y;
        bool stomping = (pVel.y > 0) && (playerBottom - enemyTop < 20);

        if (e.type == ENEMY_BLOB) {
            if (stomping) {
                e.dying = true; e.dyingTimer = 0.4f;
                enemiesDefeated++; player.StompBounce(); player.AddScore(2);
            } else {
                player.TakeDamage();
            }
        } else {
            // ENEMY_DRILLGOLEM: stomp deals 1 damage (needs 2 stomps or 1 bullet)
            if (stomping && e.stunnedTimer <= 0) {
                e.health -= 1;
                e.stunnedTimer = 0.7f;
                player.StompBounce();
                if (e.health <= 0) {
                    e.dying = true; e.dyingTimer = 0.5f;
                    enemiesDefeated++; player.AddScore(3);
                } else {
                    player.AddScore(1);
                }
            } else if (!stomping) {
                player.TakeDamage();
            }
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
                player.AddScore(3);
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

        // Draw dying flash
        if (e.dying) {
            DrawRectangle((int)sr.x-2,(int)sr.y-2,(int)sr.width+4,(int)sr.height+4,Fade(ORANGE,alpha));
        }

        // Stun flash for DrillGolem (hit but not dead)
        if (e.type == ENEMY_DRILLGOLEM && e.stunnedTimer > 0 && !e.dying) {
            float flashAlpha = (e.stunnedTimer / 0.7f) * 0.7f;
            DrawRectangle((int)sr.x-2,(int)sr.y-2,(int)sr.width+4,(int)sr.height+4,Fade(YELLOW,flashAlpha));
        }

        Texture2D& tex = (e.type == ENEMY_BLOB) ? enemyBlobTex : enemyDrillgolemTex;
        if (texturesLoaded && tex.id != 0) {
            Rectangle src = {0,0,(float)tex.width,(float)tex.height};
            if (e.movingRight) src.width = -src.width;
            DrawTexturePro(tex, src, sr, {0,0}, 0, tint);
        } else {
            DrawRectangleRec(sr, e.type==ENEMY_BLOB ? Fade(GREEN,alpha) : Fade(ORANGE,alpha));
        }

        // Health bar above enemy
        if (!e.dying) {
            int maxHP = (e.type == ENEMY_DRILLGOLEM) ? 2 : 1;
            Color barFull  = (e.type == ENEMY_DRILLGOLEM) ? (Color){220,100,0,220} : Fade(DARKGREEN,0.9f);
            Color barEmpty = {50,50,50,160};
            DrawRectangle((int)sr.x,(int)sr.y-8,(int)sr.width,5,barEmpty);
            DrawRectangle((int)sr.x,(int)sr.y-8,(int)(sr.width * ((float)e.health / maxHP)),5,barFull);
        }
    }
}

bool Level::IsCompleted() const { return completed; }
const std::vector<Rectangle>& Level::GetPlatforms() const { return platforms; }
