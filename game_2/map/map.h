#ifndef MAP_H
#define MAP_H
#include <stdint.h>

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 240

#define MAP_LEFT    0
#define MAP_RIGHT   240
#define MAP_TOP     0
#define MAP_BOTTOM  240

#define OBSTACLE_COUNT 3

typedef struct {
    float x;
    float y;
    float width;
    float height;
} Obstacle;

extern const Obstacle obstacles[OBSTACLE_COUNT];

uint8_t Obstacle_CircleCollide(const Obstacle* o, float cx, float cy, float radius);

void Map_MoveWithCollision(float* cx, float* cy, float radius, float move_x, float move_y);

uint8_t Map_BulletHitWall(float bx, float by, float radius);

void Map_Draw(void);

#endif // MAP_H