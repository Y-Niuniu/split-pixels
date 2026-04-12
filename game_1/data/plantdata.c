#include "plantdata.h"
#include"sprites/plants.h"
#include <string.h>


const Plantstatus_t plant_status[PLANT_TYPE_COUNT] = {
    [none]={.name="none", .max_health=0, .DPS=0, .attack_range=0, .attack_interval=0, .sprite=NULL},
    [wallnut]={.name="wallnut", .max_health=600, .DPS=0, .attack_range=0, .attack_interval=0, .sprite=(const uint8_t*)TEST_wallnut},
    [shooter]={.name="shooter", .max_health=125, .DPS=25, .attack_range=4, .attack_interval=1000, .sprite=(const uint8_t*)TEST_shooter},
    [bomb]={.name="bomb", .max_health=1, .DPS=0, .attack_range=1, .attack_interval=0, .sprite=(const uint8_t*)TEST_BOMB_FRAMES},
    [doomshroom]={.name="doomshroom", .max_health=1, .DPS=0, .attack_range=0, .attack_interval=0, .sprite=(const uint8_t*)TEST_doomshroom},
};