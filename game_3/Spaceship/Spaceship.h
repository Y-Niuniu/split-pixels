#ifndef SPACESHIP_H
#define SPACESHIP_H

#include <stdint.h>

// Spaceship structure
typedef struct {
    float x, y;        // position
    float dx, dy;      // direction vector (normalized)
    float speed;       // movement speed
    uint8_t radius;    // drawing radius
    uint8_t colour;    // ship colour index
    uint8_t active;    // is spaceship active
} Spaceship_t;

// Initialize spaceship
void Spaceship_Init(Spaceship_t* ship, float start_x, float start_y, float speed, uint8_t radius, uint8_t colour);

// Update spaceship position and handle wall collisions
void Spaceship_Update(Spaceship_t* ship, uint16_t screen_width, uint16_t screen_height);

// Draw spaceship on screen
void Spaceship_Draw(Spaceship_t* ship);

// Change direction (rotate by 90 degrees)
void Spaceship_ChangeDirection(Spaceship_t* ship);

// Deactivate spaceship
void Spaceship_Deactivate(Spaceship_t* ship);

#endif // SPACESHIP_H