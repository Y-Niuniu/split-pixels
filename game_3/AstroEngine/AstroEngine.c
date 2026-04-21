#include "AstroEngine.h"
#include <stdio.h>
#include <math.h>

// Static variables for button debouncing
static uint8_t button2_pressed = 0;
static uint8_t button3_pressed = 0;
static uint8_t button9_pressed = 0;
static uint8_t button7_pressed = 0;

// Initialize astro engine
void AstroEngine_Init(AstroEngine_t* engine, uint16_t width, uint16_t height) {
    engine->screen_width = width;
    engine->screen_height = height;
    Spaceship_Init(&engine->spaceship, 16.0f, 16.0f, 4.0f, 16);  // NW
    Spaceship_Init(&engine->spaceship2, width - 15.0f, height - 15.0f, 4.0f, 15);  // SE
    for (int i = 0; i < MAX_BULLETS; i++) {
        engine->bullets[i].active = 0;
    }
    engine->last_direction_change_tick = 0;
    engine->last_direction_change_tick2 = 0;
    engine->last_shoot_tick = 0;
    engine->last_shoot_tick2 = 0;
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
}

// Draw everything
void AstroEngine_Draw(AstroEngine_t* engine) {
    Spaceship_Draw(&engine->spaceship);
    Spaceship_Draw(&engine->spaceship2);
    for (int i = 0; i < MAX_BULLETS; i++) {
        Bullet_Draw(&engine->bullets[i]);
    }
}

// Shoot bullet from spaceship
void AstroEngine_ShootBullet(AstroEngine_t* engine, Spaceship_t* ship) {
    printf("Shooting bullet!\n");
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!engine->bullets[i].active) {
            // Start bullet ahead of the ship to avoid immediate collision
            float offset = ship->radius + engine->bullets[i].radius + 1.0f;  // small offset
            Bullet_Init(&engine->bullets[i], ship->x + ship->dx * offset, ship->y + ship->dy * offset, ship->dx, ship->dy, 10.0f, 4);
            break;
        }
    }
}

// Check collisions between bullets and spaceships
void AstroEngine_CheckCollisions(AstroEngine_t* engine) {
    // Check collision between spaceships
    if (engine->spaceship.active && engine->spaceship2.active) {
        float dx = engine->spaceship.x - engine->spaceship2.x;
        float dy = engine->spaceship.y - engine->spaceship2.y;
        float distance = sqrtf(dx * dx + dy * dy);
        float min_distance = engine->spaceship.radius + engine->spaceship2.radius;
        if (distance < min_distance) {
            // Separate them like wall collision: clamp positions without changing direction
            float overlap = min_distance - distance;
            if (distance > 0) {
                float nx = dx / distance;
                float ny = dy / distance;
                engine->spaceship.x += nx * overlap / 2.0f;
                engine->spaceship.y += ny * overlap / 2.0f;
                engine->spaceship2.x -= nx * overlap / 2.0f;
                engine->spaceship2.y -= ny * overlap / 2.0f;
            }
        }
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!engine->bullets[i].active) continue;

        // Check collision with spaceship
        if (engine->spaceship.active) {
            float dx = engine->bullets[i].x - engine->spaceship.x;
            float dy = engine->bullets[i].y - engine->spaceship.y;
            float distance = sqrtf(dx * dx + dy * dy);
            if (distance < engine->bullets[i].radius + engine->spaceship.radius) {
                Bullet_Deactivate(&engine->bullets[i]);
                Spaceship_Deactivate(&engine->spaceship);
                continue;
            }
        }

        // Check collision with spaceship2
        if (engine->spaceship2.active) {
            float dx = engine->bullets[i].x - engine->spaceship2.x;
            float dy = engine->bullets[i].y - engine->spaceship2.y;
            float distance = sqrtf(dx * dx + dy * dy);
            if (distance < engine->bullets[i].radius + engine->spaceship2.radius) {
                Bullet_Deactivate(&engine->bullets[i]);
                Spaceship_Deactivate(&engine->spaceship2);
                continue;
            }
        }
    }
}