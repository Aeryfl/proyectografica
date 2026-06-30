#include "player.h"
#include "level.h"
#include "raylib.h"
#include "texture_utils.h"
#include <algorithm>

Player::Player() : textureLoaded(false), bulletTexLoaded(false) { Init(); }
Player::~Player() {}

void Player::Init() {
    position    = { 80, 300 };
    velocity    = { 0, 0 };
    speed       = 270.0f;
    jumpForce   = 470.0f;
    gravity     = 950.0f;
    score       = 0;
    lives       = 3;
    isGrounded  = false;
    facingRight = true;
    invincible  = false;
    invincibleTimer = 0.0f;
    shootCooldown   = 0.0f;
    ecoPathMode     = true;
    walkDistance    = 0.0f;
    bullets.clear();
}

void Player::LoadTextures() {
    if (!textureLoaded) {
        texture = LoadTextureTransparent("assets/player.png");
        textureLoaded = (texture.id != 0);
    }
    if (!bulletTexLoaded) {
        bulletTex = LoadTextureTransparent("assets/bullet.png");
        bulletTexLoaded = (bulletTex.id != 0);
    }
}

void Player::UnloadTextures() {
    if (textureLoaded) { ::UnloadTexture(texture); textureLoaded = false; }
    if (bulletTexLoaded) { ::UnloadTexture(bulletTex); bulletTexLoaded = false; }
}

void Player::ResetPosition(float x, float y) {
    position = { x, y };
    velocity = { 0, 0 };
    invincible = true;
    invincibleTimer = 1.5f;
    bullets.clear();
}

bool Player::IsAlive() const { return lives > 0; }
int  Player::GetScore() const { return score; }
int  Player::GetLives() const { return lives; }
void Player::AddScore(int amount) { score += amount; }
Vector2 Player::GetVelocity() const { return velocity; }
Vector2 Player::GetPosition() const { return position; }
bool Player::GetFacingRight() const { return facingRight; }

void Player::StompBounce() {
    velocity.y = -320.0f;
    isGrounded = false;
}

void Player::TakeDamage() {
    if (invincible) return;
    lives--;
    invincible = true;
    invincibleTimer = 2.0f;
    ResetPosition(80, 200);
}

Rectangle Player::GetBounds() const {
    return { position.x, position.y, 30, 44 };
}

void Player::Shoot() {
    if (shootCooldown > 0) return;
    Bullet b;
    b.x      = facingRight ? position.x + 30 : position.x - Bullet::W;
    b.y      = position.y + 18;
    b.velX   = facingRight ? BULLET_SPEED : -BULLET_SPEED;
    b.active = true;
    bullets.push_back(b);
    shootCooldown = SHOOT_RATE;
}

void Player::UpdateBullets(float dt) {
    for (auto& b : bullets) {
        if (!b.active) continue;
        b.x += b.velX * dt;
        if (b.x < -50 || b.x > LEVEL_WIDTH + 50) b.active = false;
    }
    // Remove dead bullets to keep vector small
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b){ return !b.active; }), bullets.end());
}

void Player::Update(Level& level) {
    float dt = GetFrameTime();

    if (invincible) {
        invincibleTimer -= dt;
        if (invincibleTimer <= 0.0f) invincible = false;
    }
    if (shootCooldown > 0) shootCooldown -= dt;

    // Horizontal movement
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) { 
        velocity.x = speed; 
        facingRight = true; 
        if (isGrounded) walkDistance += abs(velocity.x) * dt;
    }
    else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) { 
        velocity.x = -speed; 
        facingRight = false; 
        if (isGrounded) walkDistance += abs(velocity.x) * dt;
    }
    else { 
        velocity.x = 0; 
        walkDistance = 0.0f; 
    }

    // Jump
    if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_SPACE)) && isGrounded) {
        velocity.y = -jumpForce;
        isGrounded = false;
    }

    // Shoot (Z key or Left Ctrl) - works on both paths
    // On eco path: shoots water/extinguisher; on econ path: shoots projectile
    if (IsKeyDown(KEY_Z) || IsKeyDown(KEY_LEFT_CONTROL)) {
        Shoot();
    }

    // Physics
    velocity.y += gravity * dt;
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    if (position.x < 0) position.x = 0;
    if (position.x > LEVEL_WIDTH - 30) position.x = LEVEL_WIDTH - 30;
    if (position.y > 600) TakeDamage();

    UpdateBullets(dt);
    ResolveCollisions(level);
}

void Player::ResolveCollisions(Level& level) {
    isGrounded = false;
    Rectangle pRect = GetBounds();
    for (const auto& plat : level.GetPlatforms()) {
        if (!CheckCollisionRecs(pRect, plat)) continue;
        float oLeft   = (pRect.x + pRect.width) - plat.x;
        float oRight  = (plat.x + plat.width)   - pRect.x;
        float oTop    = (pRect.y + pRect.height) - plat.y;
        float oBottom = (plat.y + plat.height)   - pRect.y;
        bool fromLeft = oLeft < oRight;
        bool fromTop  = oTop  < oBottom;
        float minX = fromLeft ? oLeft : oRight;
        float minY = fromTop  ? oTop  : oBottom;
        if (minX < minY) {
            position.x += fromLeft ? -minX : minX;
            velocity.x = 0;
        } else {
            if (fromTop) { position.y -= minY; velocity.y = 0; isGrounded = true; }
            else         { position.y += minY; velocity.y = 0; }
        }
        pRect = GetBounds();
    }
}

void Player::Draw(float cameraX) {
    // Draw bullets
    for (const auto& b : bullets) {
        if (!b.active) continue;
        Rectangle screenB = { b.x - cameraX, b.y, Bullet::W, Bullet::H };
        if (bulletTexLoaded && bulletTex.id != 0) {
            Rectangle src = { 0, 0, (float)bulletTex.width, (float)bulletTex.height };
            if (b.velX < 0) src.width = -src.width;
            DrawTexturePro(bulletTex, src, screenB, {0,0}, 0, WHITE);
        } else {
            DrawRectangleRec(screenB, YELLOW);
            DrawRectangle((int)screenB.x+2, (int)screenB.y+2, (int)Bullet::W-4, (int)Bullet::H-4, ORANGE);
        }
    }

    // Draw player
    if (invincible && ((int)(invincibleTimer * 10) % 2 == 0)) return;
    Rectangle dest = { position.x - cameraX, position.y, 30, 44 };
    
    if (textureLoaded && texture.id != 0) {
        Rectangle src = { 0, 0, (float)texture.width, (float)texture.height };
        if (!facingRight) src.width = -src.width;
        
        float rotation = 0.0f;
        
        // Simular 3 frames de animacion
        if (!isGrounded) {
            rotation = (velocity.y < 0) ? -5.0f : 5.0f;
            if (!facingRight) rotation = -rotation;
        } else if (abs(velocity.x) > 0.1f) {
            int cycle = ((int)(walkDistance / 35.0f)) % 4; // ciclo más lento para que se note
            if (cycle == 1) {
                rotation = 12.0f;
                dest.y -= 3.0f;
            } else if (cycle == 3) {
                rotation = -12.0f;
                dest.y -= 3.0f;
            }
        }
        
        Vector2 origin = { 15.0f, 44.0f }; // rotar desde los pies
        dest.x += 15.0f;
        dest.y += 44.0f;

        DrawTexturePro(texture, src, dest, origin, rotation, WHITE);
        
        // Restaurar dest para el arma
        dest.x -= 15.0f;
        dest.y -= 44.0f;
    } else {
        DrawRectangleRec(dest, BLUE);
        DrawRectangle((int)dest.x+8, (int)dest.y+4, 14, 12, LIGHTGRAY); // head
    }

    // Gun arm indicator (always visible since shooting works on both paths)
    float gx = facingRight ? dest.x + 28 : dest.x - 6;
    DrawRectangle((int)gx, (int)dest.y + 20, 8, 4, DARKGRAY);
}


