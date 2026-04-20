#ifndef ASTROENGINE_H
#define ASTROENGINE_H

#include "Spaceship/Spaceship.h"
#include "Bullet/Bullet.h"
#include "gpio.h"

#define MAX_BULLETS 10

// AstroEngine structure
typedef struct {
    Spaceship_t spaceship;
    Spaceship_t spaceship2;
    Bullet_t bullets[MAX_BULLETS];
    uint16_t screen_width;
    uint16_t screen_height;
    uint32_t last_direction_change_tick;  // for spaceship
    uint32_t last_direction_change_tick2; // for spaceship2
    uint32_t last_shoot_tick;  // for spaceship
    uint32_t last_shoot_tick2; // for spaceship2
} AstroEngine_t;

// Initialize astro engine
void AstroEngine_Init(AstroEngine_t* engine, uint16_t width, uint16_t height);

// Update engine (spaceship movement and input)
void AstroEngine_Update(AstroEngine_t* engine);

// Draw everything
void AstroEngine_Draw(AstroEngine_t* engine);

// Shoot bullet from spaceship
void AstroEngine_ShootBullet(AstroEngine_t* engine, Spaceship_t* ship);

// Check collisions between bullets and spaceships
void AstroEngine_CheckCollisions(AstroEngine_t* engine);

#endif // ASTROENGINE_H