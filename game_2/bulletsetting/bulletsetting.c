#include <math.h>
#include "LCD.h"
#include "bulletsetting.h"

void the_Bullet_Init(Bullet* b, float x, float y, float dx, float dy, float speed, int damage, uint8_t radius, BULLET_TEAM team, BULLET_TYPE type, int explosion_damage, float explosion_radius) {
    b->x = x;
    b->y = y;
    b->speed = speed;
    b->damage = damage;
    b->radius = radius;
    b->is_active = 1;
    b->team = team;
    b->type = type;
    b->explosion_damage = explosion_damage;
    b->explosion_radius = explosion_radius;

    float len = sqrtf(dx * dx + dy * dy);
    if (len > 0.001f) {
        b->dx = dx / len;
        b->dy = dy / len;
    } else {
        b->dx = 0.0f;
        b->dy = 0.0f;
    }
}

void the_Bullet_Update(Bullet* b) {
    if (!b->is_active) return;
    b->x += b->dx * b->speed;
    b->y += b->dy * b->speed;
}

void the_Bullet_Deactivate(Bullet* b) {
    b->is_active = 0;
}

void the_Bullet_Draw(Bullet* b) {
    if (!b->is_active) return;

    uint16_t color = 0;
    switch (b->type) {
        case BULLET_TYPE_NORMAL: color = 2; break;
        case BULLET_TYPE_ROCKET: color = 13; break;
        case BULLET_TYPE_ENEMY:  color = 4; break;
    }

    LCD_Draw_Circle((uint16_t)b->x, (uint16_t)b->y, b->radius, color, 1);
}