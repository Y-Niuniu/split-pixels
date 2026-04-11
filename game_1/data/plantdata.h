#ifndef PLANTDATA_H
#define PLANTDATA_H
#include <stdint.h>


//define plant type 
typedef enum{
    none,
    wallnut,
    shooter,
    bomb,
    doomshroom,
    count//represent the number of plant type
}PlantType_t;

//static plant status
typedef struct{
const char* name;
int max_health;
int DPS;
int attack_range;
int attack_interval;
const uint8_t *sprite;
}Plantstatus_t;

extern const Plantstatus_t plant_status[count];

#endif
