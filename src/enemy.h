#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"

enum EnemyType {
    ENEMY_BLOB,        // Eco path: contamination blobs  - killed by stomping (1 hit)
    ENEMY_DRILLGOLEM   // Econ path: oil drill golems    - stomp damages (2 stomps), bullets kill (1 hit)
};

struct Enemy {
    Rectangle rect;
    float patrolMinX;
    float patrolMaxX;
    float speed;
    bool movingRight;
    bool active;
    EnemyType type;
    bool dying;
    float dyingTimer;
    int health;          // Blobs = 1, DrillGolems = 2 (stomp deals 1 dmg, bullet kills)
    float stunnedTimer;  // Brief stun flash after being stomped (but not killed)
    // For drillgolem: chase behavior
    bool isChasing;
};

#endif // ENEMY_H
