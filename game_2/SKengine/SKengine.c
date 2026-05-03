#include "SKengine.h"
#include <math.h>
#include <stdlib.h>
#include "InputHandler.h"

extern InputState current_input;
extern Joystick_cfg_t joystick_cfg;
extern Joystick_cfg_t joystick_cfg2;
extern Joystick_t joystick_data;
extern Joystick_t joystick_data2;

Player player1, player2;
Enemy  enemies[MAX_ENEMIES];
Bullet player_bullet_pool[MAX_BULLETS_PLAYER];
Bullet enemy_bullet_pool[MAX_BULLETS_ENEMY];
WaveDesign wavedesign;
int team_score = 0;
static float last_explosion_x = 0;
static float last_explosion_y = 0;
static uint32_t explosion_expire = 0;

static void Engine_HandleInput(void);
static void Engine_UpdatePlayers(void);
static void Engine_UpdateEnemies(void);
static void Engine_UpdateBullets(void);
static void Engine_CheckCollisions(void);
static void Engine_PlayerShoot(Player* p, Bullet pool[], int pool_size);
static void Engine_SwitchWeapon(Player* p);
static Enemy* Engine_FindNearestEnemy(float x, float y);
static uint8_t Circle_Overlap(float x1, float y1, float r1, float x2, float y2, float r2);
static void Engine_RocketExplode(float ex, float ey);
static void Engine_TryRevive(Player* p);

void SKengine_Init(void) {
    Player_Init(&player1, 40.0f,  120.0f, "P1");
    Player_Init(&player2, 200.0f, 120.0f, "P2");

    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].is_active = 0;
    }

    for (int i = 0; i < MAX_BULLETS_PLAYER; i++) {
        player_bullet_pool[i].is_active = 0;
    }
    for (int i = 0; i < MAX_BULLETS_ENEMY; i++) {
        enemy_bullet_pool[i].is_active = 0;
    }

    WaveDesign_Init(&wavedesign);
    WaveDesign_PlanWave(&wavedesign, &player1, &player2);
}

static void Engine_HandleInput(void) {
    Joystick_Read(&joystick_cfg,  &joystick_data);
    Joystick_Read(&joystick_cfg2, &joystick_data2);

    float p1_mx = 0.0f, p1_my = 0.0f;
    float p1_speed = player1.speed * joystick_data.magnitude;
    switch (joystick_data.direction) {
        case N:  p1_mx =  0;              p1_my = -p1_speed;         break;
        case S:  p1_mx =  0;              p1_my =  p1_speed;         break;
        case E:  p1_mx =  p1_speed;       p1_my =  0;                break;
        case W:  p1_mx = -p1_speed;       p1_my =  0;                break;
        case NE: p1_mx =  p1_speed*0.707f; p1_my = -p1_speed*0.707f; break;
        case NW: p1_mx = -p1_speed*0.707f; p1_my = -p1_speed*0.707f; break;
        case SE: p1_mx =  p1_speed*0.707f; p1_my =  p1_speed*0.707f; break;
        case SW: p1_mx = -p1_speed*0.707f; p1_my =  p1_speed*0.707f; break;
        default: break;
    }
    if (player1.is_active) {
        Map_MoveWithCollision(&player1.x, &player1.y, player1.radius, p1_mx, p1_my);
    }

    float p2_mx = 0.0f, p2_my = 0.0f;
    float p2_speed = player2.speed * joystick_data2.magnitude;
    switch (joystick_data2.direction) {
        case N:  p2_mx =  0;               p2_my = -p2_speed;         break;
        case S:  p2_mx =  0;               p2_my =  p2_speed;         break;
        case E:  p2_mx =  p2_speed;        p2_my =  0;                break;
        case W:  p2_mx = -p2_speed;        p2_my =  0;                break;
        case NE: p2_mx =  p2_speed*0.707f; p2_my = -p2_speed*0.707f;  break;
        case NW: p2_mx = -p2_speed*0.707f; p2_my = -p2_speed*0.707f;  break;
        case SE: p2_mx =  p2_speed*0.707f; p2_my =  p2_speed*0.707f;  break;
        case SW: p2_mx = -p2_speed*0.707f; p2_my =  p2_speed*0.707f;  break;
        default: break;
    }
    if (player2.is_active) {
        Map_MoveWithCollision(&player2.x, &player2.y, player2.radius, p2_mx, p2_my);
    }

    if (current_input.btn6_pressed) {
    if (!player1.is_active) {
        Engine_TryRevive(&player1);
    } else {
        Engine_PlayerShoot(&player1, player_bullet_pool, MAX_BULLETS_PLAYER);
    }
}

    if (current_input.btn2_pressed && player1.is_active) {
        Engine_SwitchWeapon(&player1);
    }

    if (current_input.btn9_pressed) {
    if (!player2.is_active) {
        Engine_TryRevive(&player2);
    } else {
        Engine_PlayerShoot(&player2, player_bullet_pool, MAX_BULLETS_PLAYER);
    }
}

    if (current_input.btn7_pressed && player2.is_active) {
        Engine_SwitchWeapon(&player2);
    }
}

static Enemy* Engine_FindNearestEnemy(float x, float y) {
    Enemy* target = NULL;
    float min_dist = 999999.0f;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].is_active) continue;
        float dx = enemies[i].x - x;
        float dy = enemies[i].y - y;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist < min_dist) {
            min_dist = dist;
            target = &enemies[i];
        }
    }
    return target;
}

static void Engine_SwitchWeapon(Player* p) {
    switch (p->current_weapon) {
        case WEAPON_PISTOL:  p->current_weapon = WEAPON_RIFLE;   break;
        case WEAPON_RIFLE:   p->current_weapon = WEAPON_SHOTGUN; break;
        case WEAPON_SHOTGUN: p->current_weapon = WEAPON_ROCKET;  break;
        case WEAPON_ROCKET:  p->current_weapon = WEAPON_PISTOL;  break;
    }
}

static void Engine_PlayerShoot(Player* p, Bullet pool[], int pool_size) {
    Enemy* target = Engine_FindNearestEnemy(p->x, p->y);
    if (target == NULL) return;

    float dx = target->x - p->x;
    float dy = target->y - p->y;

    switch (p->current_weapon) {

        case WEAPON_PISTOL:
            the_Bullet_Fire(pool, pool_size, p->x, p->y, dx, dy,
                            20.0f, 1, 2, BULLET_TEAM_PLAYER, BULLET_TYPE_NORMAL, 0, 0.0f);
            break;

        case WEAPON_RIFLE:
            p->burst_active    = 1;
            p->burst_remaining = 2;
            p->burst_next_time = HAL_GetTick() + 120;
            the_Bullet_Fire(pool, pool_size, p->x, p->y, dx, dy,
                            20.0f, 1, 2, BULLET_TEAM_PLAYER, BULLET_TYPE_NORMAL, 0, 0.0f);
            break;

        case WEAPON_SHOTGUN: {
            float base_angle = atan2f(dy, dx);
            float spread = 25.0f * (3.14159f / 180.0f);
            float step   = spread / 6.0f;
            for (int i = 0; i < 7; i++) {
                float angle = base_angle - (spread / 2.0f) + step * i;
                the_Bullet_Fire(pool, pool_size, p->x, p->y,
                                cosf(angle), sinf(angle),
                                10.0f, 1, 2,
                                BULLET_TEAM_PLAYER, BULLET_TYPE_NORMAL, 0, 0.0f);
            }
            break;
        }

        case WEAPON_ROCKET:
            the_Bullet_Fire(pool, pool_size, p->x, p->y, dx, dy,
                            5.0f, 1, 4, BULLET_TEAM_PLAYER, BULLET_TYPE_ROCKET, 2, 25.0f);
            break;
    }
}

static void Engine_UpdatePlayers(void) {
    if (player1.burst_active) {
        if (HAL_GetTick() >= player1.burst_next_time && player1.burst_remaining > 0) {
            Enemy* target = Engine_FindNearestEnemy(player1.x, player1.y);
            if (target) {
                float dx = target->x - player1.x;
                float dy = target->y - player1.y;
                the_Bullet_Fire(player_bullet_pool, MAX_BULLETS_PLAYER,
                                player1.x, player1.y, dx, dy,
                                20.0f, 1, 2, BULLET_TEAM_PLAYER, BULLET_TYPE_NORMAL, 0, 0.0f);
            }
            player1.burst_remaining--;
            if (player1.burst_remaining <= 0) {
                player1.burst_active = 0;
            } else {
                player1.burst_next_time = HAL_GetTick() + 120;
            }
        }
    }

    if (player2.burst_active) {
        if (HAL_GetTick() >= player2.burst_next_time && player2.burst_remaining > 0) {
            Enemy* target = Engine_FindNearestEnemy(player2.x, player2.y);
            if (target) {
                float dx = target->x - player2.x;
                float dy = target->y - player2.y;
                the_Bullet_Fire(player_bullet_pool, MAX_BULLETS_PLAYER,
                                player2.x, player2.y, dx, dy,
                                20.0f, 1, 2, BULLET_TEAM_PLAYER, BULLET_TYPE_NORMAL, 0, 0.0f);
            }
            player2.burst_remaining--;
            if (player2.burst_remaining <= 0) {
                player2.burst_active = 0;
            } else {
                player2.burst_next_time = HAL_GetTick() + 120;
            }
        }
    }
}

static void Engine_UpdateEnemies(void) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy* e = &enemies[i];
        if (!e->is_active) continue;

        Enemy_UpdateMovement(e, player1.x, player1.y, player1.is_active, player2.x, player2.y, player2.is_active);
        float move_x = e->move_dx * e->current_speed;
        float move_y = e->move_dy * e->current_speed;
        Map_MoveWithCollision(&e->x, &e->y, e->radius, move_x, move_y);

        if (Enemy_TryShoot(e)) {
        float shoot_dx = 0, shoot_dy = 0;
        if (player1.is_active && player2.is_active) {
        float dx1 = player1.x - e->x, dy1 = player1.y - e->y;
        float dx2 = player2.x - e->x, dy2 = player2.y - e->y;
        float dist1 = sqrtf(dx1*dx1 + dy1*dy1);
        float dist2 = sqrtf(dx2*dx2 + dy2*dy2);
        shoot_dx = (dist1 <= dist2) ? dx1 : dx2;
        shoot_dy = (dist1 <= dist2) ? dy1 : dy2;
    } else if (player1.is_active) {
        shoot_dx = player1.x - e->x;
        shoot_dy = player1.y - e->y;
    } else if (player2.is_active) {
        shoot_dx = player2.x - e->x;
        shoot_dy = player2.y - e->y;
    } else {
        continue;
    }

            the_Bullet_Fire(enemy_bullet_pool, MAX_BULLETS_ENEMY,
                            e->x, e->y, shoot_dx, shoot_dy,
                            3.0f, 1, 2,
                            BULLET_TEAM_ENEMY, BULLET_TYPE_ENEMY, 0, 0.0f);
        }
    }
}

static void Engine_UpdateBullets(void) {
    for (int i = 0; i < MAX_BULLETS_PLAYER; i++) {
        Bullet* b = &player_bullet_pool[i];
        if (!b->is_active) continue;
        the_Bullet_Update(b);
        if (Map_BulletHitWall(b->x, b->y, b->radius)) {
            the_Bullet_Deactivate(b);
        }
    }

    for (int i = 0; i < MAX_BULLETS_ENEMY; i++) {
        Bullet* b = &enemy_bullet_pool[i];
        if (!b->is_active) continue;
        the_Bullet_Update(b);
        if (Map_BulletHitWall(b->x, b->y, b->radius)) {
            the_Bullet_Deactivate(b);
        }
    }
}

static uint8_t Circle_Overlap(float x1, float y1, float r1,
                               float x2, float y2, float r2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dist = sqrtf(dx * dx + dy * dy);
    return (dist <= r1 + r2) ? 1 : 0;
}

static void Engine_RocketExplode(float ex, float ey) {
    last_explosion_x = ex;
    last_explosion_y = ey;
    explosion_expire = HAL_GetTick() + 300;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].is_active) continue;
        float dx = enemies[i].x - ex;
        float dy = enemies[i].y - ey;
        if (sqrtf(dx*dx + dy*dy) <= 25.0f) {
            enemies[i].health -= 2;
            if (enemies[i].health <= 0) {
                if (enemies[i].boomer) {
                    Enemy_CheckExplosionDamage(&enemies[i], &player1, &player2);
                }
                Enemy_Deactivate(&enemies[i]);
                team_score++;
            }
        }
    }
}

static void Engine_CheckCollisions(void) {
    for (int i = 0; i < MAX_BULLETS_PLAYER; i++) {
        Bullet* b = &player_bullet_pool[i];
        if (!b->is_active) continue;

        for (int j = 0; j < MAX_ENEMIES; j++) {
            Enemy* e = &enemies[j];
            if (!e->is_active) continue;

            if (!Circle_Overlap(b->x, b->y, b->radius, e->x, e->y, e->radius)) continue;

            if (b->type == BULLET_TYPE_ROCKET) {
                Engine_RocketExplode(b->x, b->y);
            } else {
                e->health -= b->damage;
                if (e->health <= 0) {
                    if (e->boomer) {
                        Enemy_CheckExplosionDamage(e, &player1, &player2);
                    }
                    Enemy_Deactivate(e);
                    team_score++;
                }
            }
            the_Bullet_Deactivate(b);
            break;
        }
    }

    for (int i = 0; i < MAX_BULLETS_ENEMY; i++) {
        Bullet* b = &enemy_bullet_pool[i];
        if (!b->is_active) continue;

        if (player1.is_active &&
            Circle_Overlap(b->x, b->y, b->radius, player1.x, player1.y, player1.radius)) {
            Player_TakeDamage(&player1, b->damage);
            the_Bullet_Deactivate(b);
            continue;
        }
        if (player2.is_active &&
            Circle_Overlap(b->x, b->y, b->radius, player2.x, player2.y, player2.radius)) {
            Player_TakeDamage(&player2, b->damage);
            the_Bullet_Deactivate(b);
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy* e = &enemies[i];
        if (!e->is_active) continue;

        if (player1.is_active &&
            Circle_Overlap(e->x, e->y, e->radius, player1.x, player1.y, player1.radius)) {
            if (e->boomer) {
                Player_TakeDamage(&player1, e->collision_damage);
            }
            Enemy_Deactivate(e);
            continue;
        }
        if (player2.is_active &&
            Circle_Overlap(e->x, e->y, e->radius, player2.x, player2.y, player2.radius)) {
            if (e->boomer) {
                Player_TakeDamage(&player2, e->collision_damage);
            }
            Enemy_Deactivate(e);
        }
    }
}

static void Engine_TryRevive(Player* p) {
    if (p->is_active) return;
    if (team_score < 30) return;

    team_score -= 30;
    p->health = 7;
    p->is_active = 1;
}

void SKengine_Update(void) {
    Engine_HandleInput();
    Engine_UpdatePlayers();
    Engine_UpdateEnemies();
    Engine_UpdateBullets();
    Engine_CheckCollisions();
    WaveDesign_Update(&wavedesign, enemies, &player1, &player2);
}

void SKengine_Draw(void) {
    LCD_Fill_Buffer(0);

    Map_Draw();

    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy_Draw(&enemies[i]);
    }

    for (int i = 0; i < MAX_BULLETS_PLAYER; i++) {
        the_Bullet_Draw(&player_bullet_pool[i]);
    }
    for (int i = 0; i < MAX_BULLETS_ENEMY; i++) {
        the_Bullet_Draw(&enemy_bullet_pool[i]);
    }

    Player_Draw(&player1, 6);
    Player_Draw(&player2, 3);

    char score[16];
    sprintf(score, "Score:%d", team_score);
    LCD_printString(score, 0, 0, 1, 1);

    if (team_score >= 30) {
        if (!player1.is_active) {
            LCD_printString("BTN6 revive P1", 0, 10, 2, 1);
        }
        if (!player2.is_active) {
            LCD_printString("BTN9 revive P2", 0, 20, 2, 1);
        }
    }


    if (HAL_GetTick() < explosion_expire) {
    LCD_Draw_Circle((uint16_t)last_explosion_x, (uint16_t)last_explosion_y, 25, 13, 0);
    }

    LCD_printString("HP:P1", 0, 224, 6, 1);
    for (int i = 0; i < 7; i++) {
    if (i < player1.health) {
        LCD_Draw_Rect(0 + i * 7, 234, 6, 5, 6, 1);
    } else {
        LCD_Draw_Rect(0 + i * 7, 234, 6, 5, 6, 0);
    }
    }

    LCD_printString("HP:P2", 210, 224, 3, 1);
    for (int i = 0; i < 7; i++) {
    if (i < player2.health) {
        LCD_Draw_Rect(191 + i * 7, 234, 6, 5, 3, 1);
    } else {
        LCD_Draw_Rect(191 + i * 7, 234, 6, 5, 3, 0);
    }
    }

    WaveDesign_DrawWarnings(&wavedesign);
}