#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"

enum EnemyType {
    ENEMY_BLOB,       // Eco path: contamination blobs - killed by stomping
    ENEMY_COMUNARIO   // Econ path: community defenders - killed by bullets only
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
    // For comunario: chase behavior
    bool isChasing;
};

#endif // ENEMY_H
