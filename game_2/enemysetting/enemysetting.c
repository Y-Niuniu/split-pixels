#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enemysetting.h"

void Enemy_Init(Enemy* e, ENEMY_TYPE type, float x, float y) {
    e->type = type;
    e->x = x;
    e->y = y;
    e->is_active = 1;
    e->move_dx = 0.0f;
    e->move_dy = 0.0f;
    e->current_speed = 0.0f;
    e->is_moving = 0;
    e->move_start_time = 0;
    e->move_duration = 0;
    
    switch(type) {
        case ENEMY_NORMAL:
            e->width = 10.0f;
            e->height = 10.0f;
            e->health = 2;
            e->maxspeed = 0.5f;
            e->collision_damage = 0;

            e->shoot_delay = (rand() % 60) + 30;

            e->boomer = 0;
            e->explosion_damage = 0;
            e->explosion_radius = 0;
            break;
            
        case ENEMY_HEAVY:
            e->width = 20.0f;
            e->height = 20.0f;
            e->health = 3;
            e->maxspeed = 0.3f;
            e->collision_damage = 1;

            e->shoot_delay = (rand() % 100) + 100;
            
            e->boomer = 1;
            e->explosion_damage = 1;
            e->explosion_radius = 30.0f;
            break;
    }
}

void Enemy_Deactivate(Enemy* e) {
    e->is_active = 0;
}

uint8_t Enemy_IsInExplosionRadius(const Enemy* e, float player_x, float player_y) {
    float dx = player_x - e->x;
    float dy = player_y - e->y;
    float distance = sqrtf(dx * dx + dy * dy);
    return (distance <= e->explosion_radius) ? 1 : 0;
}

void Enemy_UpdateMovement(Enemy* e){
    if (!e->is_active) return;

    uint32_t now = HAL_GetTick();

    if (now - e->move_start_time >= e->move_duration) {

        if (rand() % 100 < 70) {
            e->is_moving = 0;
            e->move_dx = 0;
            e->move_dy = 0;
            e->current_speed = 0;
            e->move_duration = 800 + rand() % 2200;
            
        } else {
            e->is_moving = 1;

            float angle = (rand() % 360) * 3.14159f / 180.0f;
            e->move_dx = cosf(angle);
            e->move_dy = sinf(angle);

            float minspeed =0.1f;
            e->current_speed = (rand() % 101 / 100.0f) * (e->maxspeed - minspeed) + minspeed;

            e->move_duration = 300 + rand() % 1200;
        }

        e->move_start_time = now;
    }

    if (e->is_moving) {
        e->x += e->move_dx * e->current_speed;
        e->y += e->move_dy * e->current_speed;
    }
}
