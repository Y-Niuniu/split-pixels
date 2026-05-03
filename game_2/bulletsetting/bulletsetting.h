#ifndef BULLETSETTING_H
#define BULLETSETTING_H
#include <stdint.h>

#define MAX_BULLETS_PLAYER 300
#define MAX_BULLETS_ENEMY  100

typedef enum {
    BULLET_TEAM_PLAYER,
    BULLET_TEAM_ENEMY
} BULLET_TEAM;

typedef enum {
    BULLET_TYPE_NORMAL,
    BULLET_TYPE_ROCKET,
    BULLET_TYPE_ENEMY
} BULLET_TYPE;

typedef struct {
    float x;
    float y;
    float dx;
    float dy;
    float speed;
    int damage;
    uint8_t radius;
    uint8_t is_active;
    BULLET_TEAM team;
    BULLET_TYPE type;
    int explosion_damage;
    float explosion_radius;
} Bullet;

void the_Bullet_Init(Bullet* b,float x, float y, float dx, float dy, float speed, int damage, uint8_t radius, BULLET_TEAM team, BULLET_TYPE type, int explosion_damage, float explosion_radius);

void the_Bullet_Update(Bullet* b);

void the_Bullet_Deactivate(Bullet* b);

void the_Bullet_Draw(Bullet* b);

void the_Bullet_Fire(Bullet pool[], int pool_size, float x, float y, float dx, float dy, float speed, int damage, uint8_t radius, BULLET_TEAM team, BULLET_TYPE type, int explosion_damage, float explosion_radius);

#endif // BULLETSETTING_H