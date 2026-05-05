#include "AstroEngine.h"
#include "LCD.h"
#include "rng.h"
#include <stdio.h>
#include <math.h>

// Forward declarations
static void AstroEngine_HandleScoring(AstroEngine_t* engine);

// Static variables for button debouncing
static uint8_t button2_pressed = 0;
static uint8_t button3_pressed = 0;
static uint8_t button9_pressed = 0;
static uint8_t button7_pressed = 0;

// Initialize astro engine
void AstroEngine_Init(AstroEngine_t* engine, uint16_t width, uint16_t height) {
    engine->screen_width = width;
    engine->screen_height = height;
    engine->score1 = 0;
    engine->score2 = 0;
    engine->winner = 0;
    engine->game_state = GAME_STATE_PLAYING;
    engine->round_screen = ROUND_SCREEN_NONE;
    Spaceship_Init(&engine->spaceship, 16.0f, 16.0f, 4.0f, 16, 3);  // green ship
    Spaceship_Init(&engine->spaceship2, width - 15.0f, height - 15.0f, 4.0f, 15, 10);  // green ship
    for (int i = 0; i < MAX_BULLETS; i++) {
        engine->bullets[i].active = 0;
    }
    AstroEngine_LoadMap(engine, 1);
    engine->last_direction_change_tick = 0;
    engine->last_direction_change_tick2 = 0;
    engine->last_shoot_tick = 0;
    engine->last_shoot_tick2 = 0;
    engine->current_map = 1;
    engine->score1 = 0;
    engine->score2 = 0;
    engine->game_state = GAME_STATE_PLAYING;
    engine->winner = 0;
    AstroEngine_LoadMap(engine, 1);
}

// Update engine (spaceship movement and input)
void AstroEngine_Update(AstroEngine_t* engine) {
    // Check BTN2 (PC2) for spaceship direction
    uint8_t current_state2 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2);
    if (current_state2 == GPIO_PIN_RESET) {  // pressed
        if (!button2_pressed) {  // just pressed
            button2_pressed = 1;
            Spaceship_ChangeDirection(&engine->spaceship);
            engine->last_direction_change_tick = HAL_GetTick();
        } else {  // holding
            uint32_t current_tick = HAL_GetTick();
            if (current_tick - engine->last_direction_change_tick >= 25) {
                Spaceship_ChangeDirection(&engine->spaceship);
                engine->last_direction_change_tick = current_tick;
            }
        }
    } else {  // released
        button2_pressed = 0;
    }

    // Check BTN6 (PA7) for spaceship shooting
    uint8_t current_state3 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);
    if (current_state3 == GPIO_PIN_RESET && !button3_pressed) {  // just pressed
        uint32_t current_tick = HAL_GetTick();
        if (current_tick - engine->last_shoot_tick >= 500) {  // 500ms delay between shots
            AstroEngine_ShootBullet(engine, &engine->spaceship);
            engine->last_shoot_tick = current_tick;
        }
        button3_pressed = 1;
    } else if (current_state3 == GPIO_PIN_SET) {  // released
        button3_pressed = 0;
    }

    // Check BTN9 (PC5) for spaceship2 direction
    uint8_t current_state9 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5);
    if (current_state9 == GPIO_PIN_RESET) {  // pressed
        if (!button9_pressed) {  // just pressed
            button9_pressed = 1;
            Spaceship_ChangeDirection(&engine->spaceship2);
            engine->last_direction_change_tick2 = HAL_GetTick();
        } else {  // holding
            uint32_t current_tick = HAL_GetTick();
            if (current_tick - engine->last_direction_change_tick2 >= 25) {
                Spaceship_ChangeDirection(&engine->spaceship2);
                engine->last_direction_change_tick2 = current_tick;
            }
        }
    } else {  // released
        button9_pressed = 0;
    }

    // Check BTN7 (PA6) for spaceship2 shooting
    uint8_t current_state7 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);
    if (current_state7 == GPIO_PIN_RESET && !button7_pressed) {  // just pressed
        uint32_t current_tick = HAL_GetTick();
        if (current_tick - engine->last_shoot_tick2 >= 500) {  // 500ms delay between shots
            AstroEngine_ShootBullet(engine, &engine->spaceship2);
            engine->last_shoot_tick2 = current_tick;
        }
        button7_pressed = 1;
    } else if (current_state7 == GPIO_PIN_SET) {  // released
        button7_pressed = 0;
    }

    // Update spaceship
    Spaceship_Update(&engine->spaceship, engine->screen_width, engine->screen_height);
    // Update spaceship2
    Spaceship_Update(&engine->spaceship2, engine->screen_width, engine->screen_height);
    // Update bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        Bullet_Update(&engine->bullets[i], engine->screen_width, engine->screen_height);
    }

    // Check for collisions
    AstroEngine_CheckCollisions(engine);
    
    // Check obstacle collisions
    AstroEngine_CheckObstacleCollisions(engine);
}

// Draw background with stored random white stars for all maps
void AstroEngine_DrawBackground(AstroEngine_t* engine) {
    // Draw 200 stored white stars
    for (int i = 0; i < 200; i++) {
        LCD_Set_Pixel(engine->stars_x[i], engine->stars_y[i], 1);  // White color (1)
    }
}

// Draw obstacles
void AstroEngine_DrawObstacles(AstroEngine_t* engine) {
    for (int i = 0; i < engine->obstacle_count; i++) {
        if (engine->obstacles[i].type == OBSTACLE_LINE) {
            LineObstacle_t* line = &engine->obstacles[i].data.line;
            LCD_Draw_Line(line->x1, line->y1, line->x2, line->y2, 4);  // Blue color
        } else if (engine->obstacles[i].type == OBSTACLE_RECT) {
            RectObstacle_t* rect = &engine->obstacles[i].data.rect;
            LCD_Draw_Rect(rect->x, rect->y, rect->width, rect->height, 4, 1);  // Filled blue rectangle
        }
    }
}

// Draw everything
void AstroEngine_Draw(AstroEngine_t* engine) {
    AstroEngine_DrawBackground(engine);
    AstroEngine_DrawObstacles(engine);
    Spaceship_Draw(&engine->spaceship);
    Spaceship_Draw(&engine->spaceship2);
    for (int i = 0; i < MAX_BULLETS; i++) {
        Bullet_Draw(&engine->bullets[i]);
    }
}

// Simple circle-to-circle collision detection
uint8_t AstroEngine_CircleCollision(float x1, float y1, float r1, float x2, float y2, float r2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    float distance = sqrtf(dx * dx + dy * dy);
    return distance < (r1 + r2);
}

// Helper function to check spaceship vs obstacle collision - clamp position like walls
void AstroEngine_CheckSpaceshipObstacleCollision(AstroEngine_t* engine, Spaceship_t* ship) {
    for (int i = 0; i < engine->obstacle_count; i++) {
        if (engine->obstacles[i].type == OBSTACLE_LINE) {
            LineObstacle_t* line = &engine->obstacles[i].data.line;
            // Check distance to line segment
            float dx = ship->x - line->x1;
            float dy = ship->y - line->y1;
            float line_dx = line->x2 - line->x1;
            float line_dy = line->y2 - line->y1;
            float line_length = sqrtf(line_dx * line_dx + line_dy * line_dy);

            if (line_length > 0) {
                line_dx /= line_length;
                line_dy /= line_length;
            }

            float projection = dx * line_dx + dy * line_dy;
            if (projection < 0) projection = 0;
            if (projection > line_length) projection = line_length;

            float closest_x = line->x1 + projection * line_dx;
            float closest_y = line->y1 + projection * line_dy;

            dx = ship->x - closest_x;
            dy = ship->y - closest_y;
            float distance = sqrtf(dx * dx + dy * dy);

            if (distance < ship->radius) {
                // Push spaceship away from line
                if (distance > 0) {
                    float nx = dx / distance;
                    float ny = dy / distance;
                    ship->x = closest_x + nx * ship->radius;
                    ship->y = closest_y + ny * ship->radius;
                }
            }
        } else if (engine->obstacles[i].type == OBSTACLE_RECT) {
            RectObstacle_t* rect = &engine->obstacles[i].data.rect;
            // Find closest point on rectangle
            float closest_x = ship->x;
            float closest_y = ship->y;

            if (closest_x < rect->x) closest_x = rect->x;
            if (closest_x > rect->x + rect->width) closest_x = rect->x + rect->width;
            if (closest_y < rect->y) closest_y = rect->y;
            if (closest_y > rect->y + rect->height) closest_y = rect->y + rect->height;

            float dx = ship->x - closest_x;
            float dy = ship->y - closest_y;
            float distance = sqrtf(dx * dx + dy * dy);

            if (distance < ship->radius) {
                // Push spaceship away from rectangle
                if (distance > 0) {
                    float nx = dx / distance;
                    float ny = dy / distance;
                    ship->x = closest_x + nx * ship->radius;
                    ship->y = closest_y + ny * ship->radius;
                }
            }
        }
    }
}

// Helper function to check bullet vs obstacle collision - deactivates on contact
void AstroEngine_CheckBulletObstacleCollision(AstroEngine_t* engine, Bullet_t* bullet) {
    for (int i = 0; i < engine->obstacle_count; i++) {
        if (engine->obstacles[i].type == OBSTACLE_LINE) {
            LineObstacle_t* line = &engine->obstacles[i].data.line;
            // Check distance to line segment
            float dx = bullet->x - line->x1;
            float dy = bullet->y - line->y1;
            float line_dx = line->x2 - line->x1;
            float line_dy = line->y2 - line->y1;
            float line_length = sqrtf(line_dx * line_dx + line_dy * line_dy);

            if (line_length > 0) {
                line_dx /= line_length;
                line_dy /= line_length;
            }

            float projection = dx * line_dx + dy * line_dy;
            if (projection < 0) projection = 0;
            if (projection > line_length) projection = line_length;

            float closest_x = line->x1 + projection * line_dx;
            float closest_y = line->y1 + projection * line_dy;

            dx = bullet->x - closest_x;
            dy = bullet->y - closest_y;
            float distance = sqrtf(dx * dx + dy * dy);

            if (distance < bullet->radius) {
                Bullet_Deactivate(bullet);
                break;
            }
        } else if (engine->obstacles[i].type == OBSTACLE_RECT) {
            RectObstacle_t* rect = &engine->obstacles[i].data.rect;
            // Find closest point on rectangle
            float closest_x = bullet->x;
            float closest_y = bullet->y;

            if (closest_x < rect->x) closest_x = rect->x;
            if (closest_x > rect->x + rect->width) closest_x = rect->x + rect->width;
            if (closest_y < rect->y) closest_y = rect->y;
            if (closest_y > rect->y + rect->height) closest_y = rect->y + rect->height;

            float dx = bullet->x - closest_x;
            float dy = bullet->y - closest_y;
            float distance = sqrtf(dx * dx + dy * dy);

            if (distance < bullet->radius) {
                Bullet_Deactivate(bullet);
                break;
            }
        }
    }
}

// Check obstacle collisions
void AstroEngine_CheckObstacleCollisions(AstroEngine_t* engine) {
    // Check spaceship collisions with obstacles
    if (engine->spaceship.active) {
        AstroEngine_CheckSpaceshipObstacleCollision(engine, &engine->spaceship);
    }
    if (engine->spaceship2.active) {
        AstroEngine_CheckSpaceshipObstacleCollision(engine, &engine->spaceship2);
    }
    
    // Check bullet collisions with obstacles
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (engine->bullets[i].active) {
            AstroEngine_CheckBulletObstacleCollision(engine, &engine->bullets[i]);
        }
    }
}

// Shoot bullet from spaceship
void AstroEngine_ShootBullet(AstroEngine_t* engine, Spaceship_t* ship) {
    printf("Shooting bullet!\n");
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!engine->bullets[i].active) {
            // Start bullet from the ship's nose to prevent shooting through obstacles
            float offset = ship->radius;  // Start from nose
            Bullet_Init(&engine->bullets[i], ship->x + ship->dx * offset, ship->y + ship->dy * offset, ship->dx, ship->dy, 10.0f, 4);
            break;
        }
    }
}

// Load map configuration
void AstroEngine_LoadMap(AstroEngine_t* engine, uint8_t map_number) {
    engine->current_map = map_number;
    engine->obstacle_count = 0;
    
    // Generate random stars for all maps
    for (int i = 0; i < 200; i++) {
        uint32_t random_x, random_y;
        
        // Generate random x coordinate
        HAL_RNG_GenerateRandomNumber(&hrng, &random_x);
        engine->stars_x[i] = random_x % engine->screen_width;
        
        // Generate random y coordinate
        HAL_RNG_GenerateRandomNumber(&hrng, &random_y);
        engine->stars_y[i] = random_y % engine->screen_height;
    }
    
    uint16_t center_x = engine->screen_width / 2;
    uint16_t center_y = engine->screen_height / 2;
    
    if (map_number == 1) {
        // Map 1: Plus sign in center
        uint16_t plus_size = 60;  // Adjustable size
        uint16_t thickness = 8;   // Thickness of each bar
        
        // Horizontal bar of plus
        engine->obstacles[engine->obstacle_count].type = OBSTACLE_RECT;
        engine->obstacles[engine->obstacle_count].data.rect.x = center_x - plus_size;
        engine->obstacles[engine->obstacle_count].data.rect.y = center_y - (thickness / 2);
        engine->obstacles[engine->obstacle_count].data.rect.width = plus_size * 2;
        engine->obstacles[engine->obstacle_count].data.rect.height = thickness;
        engine->obstacle_count++;
        
        // Vertical bar of plus
        engine->obstacles[engine->obstacle_count].type = OBSTACLE_RECT;
        engine->obstacles[engine->obstacle_count].data.rect.x = center_x - (thickness / 2);
        engine->obstacles[engine->obstacle_count].data.rect.y = center_y - plus_size;
        engine->obstacles[engine->obstacle_count].data.rect.width = thickness;
        engine->obstacles[engine->obstacle_count].data.rect.height = plus_size * 2;
        engine->obstacle_count++;
        
    } else if (map_number == 2) {
        // Map 2: Equals sign in center
        uint16_t equals_width = 80;  // Adjustable width
        uint16_t equals_gap = 32;    // Vertical gap from center
        uint16_t thickness = 8;      // Thickness of each bar
        
        // Top bar of equals
        engine->obstacles[engine->obstacle_count].type = OBSTACLE_RECT;
        engine->obstacles[engine->obstacle_count].data.rect.x = center_x - equals_width;
        engine->obstacles[engine->obstacle_count].data.rect.y = center_y - equals_gap - (thickness / 2);
        engine->obstacles[engine->obstacle_count].data.rect.width = equals_width * 2;
        engine->obstacles[engine->obstacle_count].data.rect.height = thickness;
        engine->obstacle_count++;
        
        // Bottom bar of equals
        engine->obstacles[engine->obstacle_count].type = OBSTACLE_RECT;
        engine->obstacles[engine->obstacle_count].data.rect.x = center_x - equals_width;
        engine->obstacles[engine->obstacle_count].data.rect.y = center_y + equals_gap - (thickness / 2);
        engine->obstacles[engine->obstacle_count].data.rect.width = equals_width * 2;
        engine->obstacles[engine->obstacle_count].data.rect.height = thickness;
        engine->obstacle_count++;
        
    } else if (map_number == 3) {
        // Map 3: 4 squares closer to the center pointing inward
        uint16_t square_size = 40;  // Size of each square
        uint16_t gap = 20;          // Gap between center and squares
        
        // Top-left square (pointing down-right)
        engine->obstacles[engine->obstacle_count].type = OBSTACLE_RECT;
        engine->obstacles[engine->obstacle_count].data.rect.x = center_x - square_size - gap;
        engine->obstacles[engine->obstacle_count].data.rect.y = center_y - square_size - gap;
        engine->obstacles[engine->obstacle_count].data.rect.width = square_size;
        engine->obstacles[engine->obstacle_count].data.rect.height = square_size;
        engine->obstacle_count++;
        
        // Top-right square (pointing down-left)
        engine->obstacles[engine->obstacle_count].type = OBSTACLE_RECT;
        engine->obstacles[engine->obstacle_count].data.rect.x = center_x + gap;
        engine->obstacles[engine->obstacle_count].data.rect.y = center_y - square_size - gap;
        engine->obstacles[engine->obstacle_count].data.rect.width = square_size;
        engine->obstacles[engine->obstacle_count].data.rect.height = square_size;
        engine->obstacle_count++;
        
        // Bottom-left square (pointing up-right)
        engine->obstacles[engine->obstacle_count].type = OBSTACLE_RECT;
        engine->obstacles[engine->obstacle_count].data.rect.x = center_x - square_size - gap;
        engine->obstacles[engine->obstacle_count].data.rect.y = center_y + gap;
        engine->obstacles[engine->obstacle_count].data.rect.width = square_size;
        engine->obstacles[engine->obstacle_count].data.rect.height = square_size;
        engine->obstacle_count++;
        
        // Bottom-right square (pointing up-left)
        engine->obstacles[engine->obstacle_count].type = OBSTACLE_RECT;
        engine->obstacles[engine->obstacle_count].data.rect.x = center_x + gap;
        engine->obstacles[engine->obstacle_count].data.rect.y = center_y + gap;
        engine->obstacles[engine->obstacle_count].data.rect.width = square_size;
        engine->obstacles[engine->obstacle_count].data.rect.height = square_size;
        engine->obstacle_count++;
    }
}

// Check collisions between bullets and spaceships (circle-to-circle only)
void AstroEngine_CheckCollisions(AstroEngine_t* engine) {
    // Check collision between spaceships - push them apart instead of deactivating
    if (engine->spaceship.active && engine->spaceship2.active) {
        if (AstroEngine_CircleCollision(engine->spaceship.x, engine->spaceship.y, engine->spaceship.radius,
                                        engine->spaceship2.x, engine->spaceship2.y, engine->spaceship2.radius)) {
            // Calculate direction vector between centers
            float dx = engine->spaceship2.x - engine->spaceship.x;
            float dy = engine->spaceship2.y - engine->spaceship.y;
            float distance = sqrtf(dx * dx + dy * dy);
            
            if (distance > 0) {
                // Normalize
                float nx = dx / distance;
                float ny = dy / distance;
                
                // Push them apart by their radii
                float overlap = (engine->spaceship.radius + engine->spaceship2.radius) - distance;
                if (overlap > 0) {
                    // Move each ship away by half the overlap
                    float push = overlap / 2.0f;
                    engine->spaceship.x -= nx * push;
                    engine->spaceship.y -= ny * push;
                    engine->spaceship2.x += nx * push;
                    engine->spaceship2.y += ny * push;
                }
            }
            printf("Spaceships collided and bounced!\n");
            return;
        }
    }

    // Check bullet collisions
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!engine->bullets[i].active) continue;

        // Check collision with spaceship1
        if (engine->spaceship.active) {
            if (AstroEngine_CircleCollision(engine->bullets[i].x, engine->bullets[i].y, engine->bullets[i].radius,
                                            engine->spaceship.x, engine->spaceship.y, engine->spaceship.radius)) {
                Bullet_Deactivate(&engine->bullets[i]);
                Spaceship_Deactivate(&engine->spaceship);
                
                // Spaceship2 scores
                engine->score2++;
                printf("Spaceship 2 scores! Score: %d-%d\n", engine->score1, engine->score2);
                AstroEngine_HandleScoring(engine);
                continue;
            }
        }

        // Check collision with spaceship2
        if (engine->spaceship2.active) {
            if (AstroEngine_CircleCollision(engine->bullets[i].x, engine->bullets[i].y, engine->bullets[i].radius,
                                            engine->spaceship2.x, engine->spaceship2.y, engine->spaceship2.radius)) {
                Bullet_Deactivate(&engine->bullets[i]);
                Spaceship_Deactivate(&engine->spaceship2);
                
                // Spaceship1 scores
                engine->score1++;
                printf("Spaceship 1 scores! Score: %d-%d\n", engine->score1, engine->score2);
                AstroEngine_HandleScoring(engine);
                continue;
            }
        }
    }
}

// Handle scoring and map progression
static void AstroEngine_HandleScoring(AstroEngine_t* engine) {
    // Respawn spaceships with their original colours preserved
    Spaceship_Init(&engine->spaceship, 16.0f, 16.0f, 4.0f, 16, 3);  // Keep green for spaceship1
    Spaceship_Init(&engine->spaceship2, engine->screen_width - 15.0f, engine->screen_height - 15.0f, 4.0f, 15, 10);  // Keep gold for spaceship2
    
    if (engine->current_map == 1) {
        AstroEngine_LoadMap(engine, 2);
        engine->round_screen = ROUND_SCREEN_NEXT;
        printf("Next round: Map 2\n");
    } else if (engine->current_map == 2) {
        if (engine->score1 > engine->score2) {
            engine->game_state = GAME_STATE_END;
            engine->winner = 1;
            printf("Game Over! Spaceship 1 wins!\n");
        } else if (engine->score2 > engine->score1) {
            AstroEngine_LoadMap(engine, 3);
            engine->round_screen = ROUND_SCREEN_FINAL;
            printf("Final round: Map 3\n");
        } else {
            AstroEngine_LoadMap(engine, 3);
            engine->round_screen = ROUND_SCREEN_FINAL;
            printf("Final round: Map 3\n");
        }
    } else if (engine->current_map == 3) {
        engine->game_state = GAME_STATE_END;
        if (engine->score1 > engine->score2) {
            engine->winner = 1;
            printf("Game Over! Spaceship 1 wins!\n");
        } else {
            engine->winner = 2;
            printf("Game Over! Spaceship 2 wins!\n");
        }
    }
}


