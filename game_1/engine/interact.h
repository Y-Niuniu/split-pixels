#ifndef INTERACT_H
#define INTERACT_H
#include <stdint.h>
#include <stdbool.h>
#include "data/zombiedata.h"
//define a zombie instance
typedef struct{
zombietype_t type;
int x;
int y;
int row; 
int health;
bool is_alive;
uint8_t attack_interval;
}zombieinstance_t;

void Zombie_Init(void);
void Zombie_Update(uint32_t delta_ms);
void Zombie_Render(void);
void Damage_Determination(void);
void Score_Add(void);
void Score_Get(void);
uint32_t Game_time_pass(void);
bool IsVictory(void);
#endif