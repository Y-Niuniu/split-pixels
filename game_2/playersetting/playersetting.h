#ifndef PLAYERSETTING_H
#define PLAYERSETTING_H

#include <stdint.h>

typedef struct {
    float x;
    float y;
    uint8_t is_active;
    int health;
    float speed;
    uint8_t radius;
    int score;
} Player;

void Player_Init(Player* p, float start_x, float start_y);