#ifndef INTERACT_H
#define INTERACT_H
#include <stdint.h>
#include <stdbool.h>
#include "data/zombiedata.h"

#define MAX_ZOMBIES 24 //we have 3*5=15 zombies this for Reserved for the upper limit
//define a zombie instance
typedef struct{
zombietype_t type;
int x;
int y;
int row; 
int health;
bool is_alive;
bool is_attacking;
uint8_t attack_interval;
}zombieinstance_t;

extern zombieinstance_t zombie_instances[MAX_ZOMBIES];

void Zombie_Init(void);
void Zombie_Update(uint32_t delta_ms);
void Zombie_Render(void);
void Damage_Determination(void);
void Score_Add(int score);
int Score_Get(void);
uint32_t Game_time_pass(void);
bool IsVictory(void);
bool IsGameOver(void);
#endif