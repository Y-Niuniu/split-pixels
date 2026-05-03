#ifndef PLAYERSETTING_H
#define PLAYERSETTING_H
#include <stdint.h>
#include "weaponsetting/weaponsetting.h"

typedef struct {
    const char* name;
    float x;
    float y;
    float radius;
    uint8_t is_active;
    int health;
    float speed;
    int score;
    WEAPON_TYPE current_weapon;

    uint8_t  burst_active;
    int      burst_remaining;
    uint32_t burst_next_time;
} Player;

void Player_Init(Player* p, float start_x, float start_y, const char* name); //Initialize player with starting position

void Player_TakeDamage(Player* p, int damage);  //Apply damage to player and handle invincibility frames

void Player_Draw(Player* p, uint8_t color);  //Draw player on screen with specified color
#endif  //PLAYERSETTING_H