#include "zombiedata.h"
#include "sprites/zombies.h"
#include <string.h>
const Zombiestatus_t zombie_status[count] = {
    [normal]={.name="normal", .max_health=250, .DPS=25, .speed_pix_per_sec=15.0f, .score=150, .sprite=(const uint8_t*)TEST_ZOMBIE1_FRAMES[0]},
    [bucket]={.name="bucket", .max_health=400, .DPS=30, .speed_pix_per_sec=10.0f, .score=350, .sprite=(const uint8_t*)TEST_ZOMBIE2_FRAMES[0]},
};
