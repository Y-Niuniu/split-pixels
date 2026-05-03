#ifndef WAVEDESIGN_H
#define WAVEDESIGN_H
#include <stdint.h>
#include "enemysetting/enemysetting.h"
#include "playersetting/playersetting.h"

#define MAX_ENEMIES          40
#define WAVE_REST_MS         4000
#define SPAWN_WARNING_MS     2000
#define SPAWN_BLINK_INTERVAL 300
#define SPAWN_MIN_DIST       60.0f
#define SPAWN_STAGGER_MS     700

typedef struct {
    float x;
    float y;
    ENEMY_TYPE type;
    uint32_t spawn_time;
    uint8_t spawned;
    uint8_t warning_on;
} SpawnEntry;

typedef struct {
    int wave;
    SpawnEntry entries[MAX_ENEMIES];
    int entry_count;
    uint8_t wave_active;
    uint32_t rest_start_time;
    uint8_t resting;
} WaveDesign;

void WaveDesign_Init(WaveDesign* w);
void WaveDesign_PlanWave(WaveDesign* w, Player* p1, Player* p2);
void WaveDesign_Update(WaveDesign* w, Enemy enemies[], Player* p1, Player* p2);
uint8_t WaveDesign_WaveCleared(WaveDesign* w, Enemy enemies[]);
void WaveDesign_DrawWarnings(WaveDesign* w);

#endif // WAVEDESIGN_H