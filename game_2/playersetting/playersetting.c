#include "playersetting.h"
#include "stm32l4xx_hal.h"
#include "LCD.h"

void Player_Init(Player* p, float start_x, float start_y, const char* name) {
    p->name = name;
    p->x = start_x;
    p->y = start_y;
    p->radius = 3.0f;
    p->is_active = 1;
    p->health = 7;
    p->speed = 7.0f;
    p->current_weapon = WEAPON_PISTOL;

    p->burst_active    = 0;
    p->burst_remaining = 0;
    p->burst_next_time = 0;
}

void Player_TakeDamage(Player* p, int damage) {
    if (!p->is_active) return;

    p->health -= damage;

    if (p->health <= 0) {
        p->health = 0;
        p->is_active = 0;
    }
}

void Player_Draw(Player* p, uint8_t color) {
    if (!p->is_active) return;
    uint16_t draw_x = (uint16_t)(p->x - 8);
    uint16_t draw_y = (uint16_t)(p->y - 4);
    LCD_printString(p->name, draw_x, draw_y, color, 1);
}