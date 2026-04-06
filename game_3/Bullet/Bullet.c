#include "Bullet.h"
#include "LCD.h"
#include <math.h>

// Initialize bullet
void Bullet_Init(Bullet_t* bullet, float start_x, float start_y, float dx, float dy, float speed, uint8_t radius) {
    bullet->x = start_x;
    bullet->y = start_y;
    bullet->dx = dx;
    bullet->dy = dy;
    bullet->speed = speed;
    bullet->radius = radius;
    bullet->active = 1;
}

// Update bullet position
void Bullet_Update(Bullet_t* bullet, uint16_t screen_width, uint16_t screen_height) {
    if (!bullet->active) return;

    // Move bullet
    bullet->x += bullet->dx * bullet->speed;
    bullet->y += bullet->dy * bullet->speed;

    // Deactivate if out of screen
    if (bullet->x < 0 || bullet->x > screen_width || bullet->y < 0 || bullet->y > screen_height) {
        bullet->active = 0;
    }
}

// Draw bullet on screen
void Bullet_Draw(Bullet_t* bullet) {
    if (!bullet->active) return;

    // Draw as a small circle (filled)
    LCD_Draw_Circle((uint16_t)bullet->x, (uint16_t)bullet->y, bullet->radius, 2, 1);
}

// Deactivate bullet
void Bullet_Deactivate(Bullet_t* bullet) {
    bullet->active = 0;
}