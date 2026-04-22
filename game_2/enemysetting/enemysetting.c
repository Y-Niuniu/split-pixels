#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enemysetting.h"

static const char* NAME_PART_A[] = {

};

static const char* NAME_PART_B[] = {
    
};

#define NAME_COUNT_A (sizeof(NAME_PART_A) / sizeof(const char*))
#define NAME_COUNT_B (sizeof(NAME_PART_B) / sizeof(const char*))

void Enemy_RandomName(char* output, int max_len) {
    int index_a = rand() % NAME_COUNT_A;
    int index_b = rand() % NAME_COUNT_B;
    
    snprintf(output, max_len, "%s %s", 
             NAME_PART_A[index_a], 
             NAME_PART_B[index_b]);
}

void Enemy_Init(Enemy* e, ENEMY_TYPE type, float x, float y) {
    e->type = type;
    e->x = x;
    e->y = y;
    e->is_active = 1;
    
    Enemy_RandomName(e->name, sizeof(e->name));
    
    switch(type) {
        case ENEMY_NORMAL:
            e->health = 2;
            e->speed = 1f;
            e->collision_damage = 0;

            e->shoot_delay = (rand() % 60) + 30;

            e->boomer = 0;
            e->will_explode_on_death = 0;
            e->explosion_damage = 0;
            e->explosion_radius = 0;
            break;
            
        case ENEMY_HEAVY:
            e->health = 4;
            e->speed = 0.7f;
            e->collision_damage = 1;

            e->shoot_delay = (rand() % 100) + 100;
            
            e->boomer = 1;
            e->will_explode_on_death = 1;
            e->explosion_damage = 1;
            e->explosion_radius = 15;
            break;
    }
}