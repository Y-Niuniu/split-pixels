#ifndef ZOMBIEDATA_H
#define ZOMBIEDATA_H
#include <stdint.h>

typedef enum {
  normal,
  bucket,
    ZOMBIE_TYPE_COUNT
}zombietype_t;

typedef struct{
const char* name;
int max_health;
int DPS;
float speed_pix_per_sec;
int score;
uint8_t h;
uint8_t w;
const uint8_t *sprite;
}Zombiestatus_t;

extern const Zombiestatus_t zombie_status[ZOMBIE_TYPE_COUNT];
#endif