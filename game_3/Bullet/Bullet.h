#ifndef BULLET_H
#define BULLET_H

#include <stdint.h>

// Bullet structure
typedef struct {
    float x, y;        // position
    float dx, dy;      // direction vector (normalized)
    float speed;       // movement speed
    uint8_t radius;    // drawing radius
    uint8_t active;    // is bullet active
} Bullet_t;

// Initialize bullet
void Bullet_Init(Bullet_t* bullet, float start_x, float start_y, float dx, float dy, float speed, uint8_t radius);

// Update bullet position
void Bullet_Update(Bullet_t* bullet, uint16_t screen_width, uint16_t screen_height);

// Draw bullet on screen
void Bullet_Draw(Bullet_t* bullet);

// Deactivate bullet
void Bullet_Deactivate(Bullet_t* bullet);

#endif // BULLET_H