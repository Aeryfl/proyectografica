#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <vector>

class Level;

struct Bullet {
    float x, y;
    float velX;
    bool active;
    static constexpr float W = 18.0f;
    static constexpr float H = 8.0f;
};

class Player {
public:
    Player();
    ~Player();

    void Init();
    void LoadTextures();
    void UnloadTextures();
    void Update(Level& level);
    void Draw(float cameraX = 0.0f);
    void ResetPosition(float x, float y);

    bool IsAlive() const;
    int GetScore() const;
    int GetLives() const;
    void AddScore(int amount);
    void TakeDamage();
    void StompBounce();

    Rectangle GetBounds() const;
    Vector2 GetVelocity() const;
    Vector2 GetPosition() const;
    bool GetFacingRight() const;

    std::vector<Bullet>& GetBullets() { return bullets; }
    void SetEcoMode(bool eco) { ecoPathMode = eco; }

private:
    Vector2 position;
    Vector2 velocity;
    float speed;
    float jumpForce;
    float gravity;
    int score;
    int lives;
    bool isGrounded;
    bool facingRight;
    bool invincible;
    float invincibleTimer;

    // Shooting
    std::vector<Bullet> bullets;
    float shootCooldown;
    static constexpr float SHOOT_RATE = 0.18f;
    static constexpr float BULLET_SPEED = 620.0f;
    bool ecoPathMode; // if true, space=jump only (no shooting); if false, Z=shoot

    Texture2D texture;
    Texture2D bulletTex;
    bool textureLoaded;
    bool bulletTexLoaded;

    void Shoot();
    void UpdateBullets(float dt);
    void ResolveCollisions(Level& level);
};

#endif // PLAYER_H
