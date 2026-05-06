#include "zombiedata.h"
#include "sprites/zombies.h"
#include <string.h>
const Zombiestatus_t zombie_status[ZOMBIE_TYPE_COUNT] = {
    [normal]={.name="normal", .max_health=250, .DPS=25, .speed_pix_per_sec=11.0f, .score=150,.h=36,.w=25, .sprite=(const uint8_t*)TEST_ZOMBIE1_FRAMES},
    [bucket]={.name="bucket", .max_health=400, .DPS=30, .speed_pix_per_sec=9.0f, .score=350,.h=44,.w=31, .sprite=(const uint8_t*)TEST_ZOMBIE2_FRAMES},
};