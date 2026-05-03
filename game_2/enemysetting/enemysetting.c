#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "enemysetting.h"
#include "stm32l4xx_hal.h"
#include "playersetting/playersetting.h"
#include "LCD.h"

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
    e->last_shot_time = 0;
    e->next_shot_delay = 1000 + rand() % 2000;
    
    switch(type) {
        case ENEMY_NORMAL:
            e->radius = 5.0f;
            e->health = 2;
            e->maxspeed = 20.0f;
            e->collision_damage = 0;

            e->boomer = 0;
            e->explosion_damage = 0;
            e->explosion_radius = 0;
            break;
            
        case ENEMY_HEAVY:
            e->radius = 10.0f;
            e->health = 3;
            e->maxspeed = 15.0f;
            e->collision_damage = 1;
            
            e->boomer = 1;
            e->explosion_damage = 1;
            e->explosion_radius = 20.0f;
            break;
    }
}

void Enemy_Deactivate(Enemy* e) {
    e->is_active = 0;
}

uint8_t Enemy_IsInExplosionRadius(const Enemy* e, float x, float y) {
    float dx = x - e->x;
    float dy = y - e->y;
    float distance = sqrtf(dx * dx + dy * dy);
    return (distance <= e->explosion_radius) ? 1 : 0;
}

void Enemy_CheckExplosionDamage(const Enemy* e, Player* p1, Player* p2) {
    if (!e->boomer) return;

    if (p1 && p1->is_active && Enemy_IsInExplosionRadius(e, p1->x, p1->y)) {
        Player_TakeDamage(p1, e->explosion_damage);
    }

    if (p2 && p2->is_active && Enemy_IsInExplosionRadius(e, p2->x, p2->y)) {
        Player_TakeDamage(p2, e->explosion_damage);
    }
}

void Enemy_UpdateMovement(Enemy* e, float p1_x, float p1_y, float p2_x, float p2_y) {
    if (!e->is_active) return;

    uint32_t now = HAL_GetTick();

    if (e->boomer && e->health <= 1) {
        float dx1 = p1_x - e->x, dy1 = p1_y - e->y;
        float dx2 = p2_x - e->x, dy2 = p2_y - e->y;
        float dist1 = sqrtf(dx1 * dx1 + dy1 * dy1);
        float dist2 = sqrtf(dx2 * dx2 + dy2 * dy2);

        float target_dx, target_dy;
        if (dist1 <= dist2) {
            target_dx = dx1; target_dy = dy1;
        } else {
            target_dx = dx2; target_dy = dy2;
        }

        float dist = sqrtf(target_dx * target_dx + target_dy * target_dy);
        if (dist > 0.001f) {
            e->move_dx = target_dx / dist;
            e->move_dy = target_dy / dist;
        }

        e->is_moving = 1;
        e->current_speed = e->maxspeed;
        e->x += e->move_dx * e->current_speed;
        e->y += e->move_dy * e->current_speed;
        return;
    }

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

            float minspeed =10.0f;
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

uint8_t Enemy_TryShoot(Enemy* e) {
    if (!e->is_active) return 0;

    if (e->boomer && e->health <= 1) return 0;

    uint32_t now = HAL_GetTick();
    uint32_t elapsed = now - e->last_shot_time;

    if (elapsed < 1000) return 0;

    uint32_t pressure_bonus = 0;
    if (elapsed > 1500) {
        pressure_bonus = elapsed - 1500;
        if (pressure_bonus > 500) {
            pressure_bonus = 500;
        }
    }

    if (elapsed >= e->next_shot_delay - pressure_bonus) {
        e->last_shot_time = now;
        e->next_shot_delay = 1000 + rand() % 2000;
        return 1;
    }

    return 0;
}

void Enemy_Draw(Enemy* e) {
    if (!e->is_active) return;

    uint16_t color = 0;
    switch (e->type) {
        case ENEMY_NORMAL:
            color = 6;
            break;
        case ENEMY_HEAVY:
            color = (e->health <= 1) ? 10 : 2;
            break;
    }
    LCD_Draw_Circle((uint16_t)e->x, (uint16_t)e->y, (uint16_t)e->radius, color, 0);
}
