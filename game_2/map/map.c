#include "map.h"
#include "LCD.h"
#include <math.h>


#define OBSTACLE_COLOR  2

const Obstacle obstacles[OBSTACLE_COUNT] = {
    { 32.0f, 154.0f, 32.0f, 32.0f },
    { 64.0f,  64.0f, 64.0f, 32.0f },
    {176.0f, 128.0f, 32.0f, 64.0f },
};

uint8_t Obstacle_CircleCollide(const Obstacle* o, float cx, float cy, float radius) {

    float nearest_x = cx;
    float nearest_y = cy;

    if      (cx < o->x)             nearest_x = o->x;
    else if (cx > o->x + o->width)  nearest_x = o->x + o->width;

    if      (cy < o->y)              nearest_y = o->y;
    else if (cy > o->y + o->height)  nearest_y = o->y + o->height;

    float dx = cx - nearest_x;
    float dy = cy - nearest_y;
    return (dx * dx + dy * dy <= radius * radius) ? 1 : 0;
}

void Map_MoveWithCollision(float* cx, float* cy, float radius,
                           float move_x, float move_y) {

    float new_x = *cx + move_x;
    uint8_t block_x = 0;

    if (new_x - radius < MAP_LEFT || new_x + radius > MAP_RIGHT) {
        block_x = 1;
    }

    if (!block_x) {
        for (int i = 0; i < OBSTACLE_COUNT; i++) {
            if (Obstacle_CircleCollide(&obstacles[i], new_x, *cy, radius)) {
                block_x = 1;
                break;
            }
        }
    }

    if (!block_x) *cx = new_x;

    float new_y = *cy + move_y;
    uint8_t block_y = 0;

    if (new_y - radius < MAP_TOP || new_y + radius > MAP_BOTTOM) {
        block_y = 1;
    }

    if (!block_y) {
        for (int i = 0; i < OBSTACLE_COUNT; i++) {
            if (Obstacle_CircleCollide(&obstacles[i], *cx, new_y, radius)) {
                block_y = 1;
                break;
            }
        }
    }

    if (!block_y) *cy = new_y;
}

uint8_t Map_BulletHitWall(float bx, float by, float radius) {
    if (bx - radius < MAP_LEFT  || bx + radius > MAP_RIGHT ||
        by - radius < MAP_TOP   || by + radius > MAP_BOTTOM) {
        return 1;
    }

    for (int i = 0; i < OBSTACLE_COUNT; i++) {
        if (Obstacle_CircleCollide(&obstacles[i], bx, by, radius)) {
            return 1;
        }
    }

    return 0;
}

void Map_Draw(void) {
    for (int i = 0; i < OBSTACLE_COUNT; i++) {
        const Obstacle* o = &obstacles[i];
        LCD_Draw_Rect((uint16_t)o->x, (uint16_t)o->y, (uint16_t)o->width,(uint16_t)o->height, OBSTACLE_COLOR, 1);
    }
}