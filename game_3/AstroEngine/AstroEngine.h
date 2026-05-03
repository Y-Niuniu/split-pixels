#ifndef ASTROENGINE_H
#define ASTROENGINE_H

#include "Spaceship/Spaceship.h"
#include "Bullet/Bullet.h"
#include "gpio.h"

#define MAX_BULLETS 10
#define MAX_OBSTACLES 20

// Obstacle types
typedef enum {
    OBSTACLE_LINE,
    OBSTACLE_RECT
} ObstacleType_t;

typedef enum {
    ROUND_SCREEN_NONE = 0,
    ROUND_SCREEN_NEXT,
    ROUND_SCREEN_FINAL
} RoundScreen_t;

// Line obstacle (for plus and equals signs)
typedef struct {
    int16_t x1, y1, x2, y2;  // line endpoints
} LineObstacle_t;

// Rectangle obstacle (for corner squares)
typedef struct {
    int16_t x, y, width, height;  // rectangle position and size
} RectObstacle_t;

// Generic obstacle
typedef struct {
    ObstacleType_t type;
    union {
        LineObstacle_t line;
        RectObstacle_t rect;
    } data;
} Obstacle_t;

// Map configuration
typedef struct {
    Obstacle_t obstacles[MAX_OBSTACLES];
    uint8_t obstacle_count;
    uint8_t map_number;  // 1, 2, or 3
} MapConfig_t;

// Game states
typedef enum {
    GAME_STATE_PLAYING,
    GAME_STATE_END
} GameState_t;

// AstroEngine structure
typedef struct {
    Spaceship_t spaceship;
    Spaceship_t spaceship2;
    Bullet_t bullets[MAX_BULLETS];
    Obstacle_t obstacles[MAX_OBSTACLES];
    uint8_t obstacle_count;
    uint16_t screen_width;
    uint16_t screen_height;
    uint32_t last_direction_change_tick;  // for spaceship
    uint32_t last_direction_change_tick2; // for spaceship2
    uint32_t last_shoot_tick;  // for spaceship
    uint32_t last_shoot_tick2; // for spaceship2
    uint8_t current_map;  // 1, 2, or 3
    uint8_t score1;  // spaceship 1 score
    uint8_t score2;  // spaceship 2 score
    RoundScreen_t round_screen;
    GameState_t game_state;
    uint8_t winner;  // 1 or 2, 0 if no winner yet
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

// Load map configuration
void AstroEngine_LoadMap(AstroEngine_t* engine, uint8_t map_number);

// Draw obstacles
void AstroEngine_DrawObstacles(AstroEngine_t* engine);

// Check obstacle collisions
void AstroEngine_CheckObstacleCollisions(AstroEngine_t* engine);

#endif // ASTROENGINE_H