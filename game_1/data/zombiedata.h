#ifndef ZOMBIEDATA_H
#define ZOMBIEDATA_H
#include <stdint.h>


typedef enum {
  normal,
  bucket,
  count
}zombietype_t;

typedef struct{
const char* name;
int max_health;
int DPS;
int speed_pix_per_sec;
int score;
const uint8_t *sprite;
}Zombiestatus_t;

extern const Zombiestatus_t zombie_status[count];
#endif
