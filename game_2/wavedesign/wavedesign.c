#include "wavedesign.h"
#include "map/map.h"
#include "LCD.h"
#include "stm32l4xx_hal.h"
#include <stdlib.h>
#include <math.h>

#define BASE_ENEMIES        3
#define ENEMIES_PER_WAVE    2
#define HEAVY_START_WAVE    3
#define HEAVY_CHANCE_BASE   20
#define HEAVY_CHANCE_INC    10
#define HEAVY_CHANCE_MAX    40

static void Find_SpawnPosition(float* out_x, float* out_y, Player* p1, Player* p2) {
    int attempts = 0;
    float x, y;

    do {
        x = SPAWN_MIN_DIST + (float)(rand() % (int)(SCREEN_WIDTH  - SPAWN_MIN_DIST * 2));
        y = SPAWN_MIN_DIST + (float)(rand() % (int)(SCREEN_HEIGHT - SPAWN_MIN_DIST * 2));

        float dx1 = x - p1->x, dy1 = y - p1->y;
        float dx2 = x - p2->x, dy2 = y - p2->y;
        float dist1 = sqrtf(dx1 * dx1 + dy1 * dy1);
        float dist2 = sqrtf(dx2 * dx2 + dy2 * dy2);

        uint8_t in_obstacle = 0;
        for (int i = 0; i < OBSTACLE_COUNT; i++) {
            if (Obstacle_CircleCollide(&obstacles[i], x, y, 10.0f)) {
                in_obstacle = 1;
                break;
            }
        }

        if (dist1 >= SPAWN_MIN_DIST && dist2 >= SPAWN_MIN_DIST && !in_obstacle) break;
        attempts++;
    } while (attempts < 50);

    *out_x = x;
    *out_y = y;
}

void WaveDesign_Init(WaveDesign* w) {
    w->wave            = 0;
    w->entry_count     = 0;
    w->wave_active     = 0;
    w->resting         = 0;
    w->rest_start_time = 0;
}

void WaveDesign_PlanWave(WaveDesign* w, Player* p1, Player* p2) {
    w->wave++;

    int count = BASE_ENEMIES + (w->wave - 1) * ENEMIES_PER_WAVE;
    if (count > MAX_ENEMIES) count = MAX_ENEMIES;
    w->entry_count = count;

    int heavy_chance = 0;
    if (w->wave >= HEAVY_START_WAVE) {
        heavy_chance = HEAVY_CHANCE_BASE + (w->wave - HEAVY_START_WAVE) * HEAVY_CHANCE_INC;
        if (heavy_chance > HEAVY_CHANCE_MAX) heavy_chance = HEAVY_CHANCE_MAX;
    }

    uint32_t now = HAL_GetTick();
    for (int i = 0; i < count; i++) {
        SpawnEntry* entry = &w->entries[i];

        Find_SpawnPosition(&entry->x, &entry->y, p1, p2);

        entry->type = (rand() % 100 < heavy_chance)
                      ? ENEMY_HEAVY : ENEMY_NORMAL;

        entry->spawn_time = now + SPAWN_WARNING_MS + (i / 8) * SPAWN_STAGGER_MS;
        entry->spawned    = 0;
        entry->warning_on = 0;
    }

    w->wave_active = 1;
}

void WaveDesign_Update(WaveDesign* w, Enemy enemies[], Player* p1, Player* p2) {
    if (w->resting) {
        if (HAL_GetTick() - w->rest_start_time >= WAVE_REST_MS) {
            w->resting = 0;
            WaveDesign_PlanWave(w, p1, p2);
        }
        return;
    }

    if (!w->wave_active) return;

    uint32_t now = HAL_GetTick();

    for (int i = 0; i < w->entry_count; i++) {
        SpawnEntry* entry = &w->entries[i];
        if (entry->spawned) continue;

        if (now < entry->spawn_time) {
            entry->warning_on = ((now / SPAWN_BLINK_INTERVAL) % 2 == 0) ? 1 : 0;
            continue;
        }

        for (int j = 0; j < MAX_ENEMIES; j++) {
            if (!enemies[j].is_active) {
                Enemy_Init(&enemies[j], entry->type, entry->x, entry->y);
                entry->spawned = 1;
                break;
            }
        }
    }

    if (WaveDesign_WaveCleared(w, enemies)) {
        w->wave_active     = 0;
        w->resting         = 1;
        w->rest_start_time = HAL_GetTick();
    }
}

uint8_t WaveDesign_WaveCleared(WaveDesign* w, Enemy enemies[]) {
    for (int i = 0; i < w->entry_count; i++) {
        if (!w->entries[i].spawned) return 0;
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].is_active) return 0;
    }

    return 1;
}

void WaveDesign_DrawWarnings(WaveDesign* w) {
    if (!w->wave_active) return;

    for (int i = 0; i < w->entry_count; i++) {
        SpawnEntry* entry = &w->entries[i];
        if (entry->spawned) continue;
        if (!entry->warning_on) continue;

        uint8_t warn_radius = (entry->type == ENEMY_HEAVY) ? 7 : 5;
        LCD_Draw_Circle((uint16_t)entry->x, (uint16_t)entry->y,
                        warn_radius, 4, 0);
    }
}