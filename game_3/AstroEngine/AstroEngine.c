#include "AstroEngine.h"
#include <stdio.h>

// Static variables for button debouncing
static uint8_t button2_pressed = 0;
static uint8_t button3_pressed = 0;

// Initialize astro engine
void AstroEngine_Init(AstroEngine_t* engine, uint16_t width, uint16_t height) {
    engine->screen_width = width;
    engine->screen_height = height;
    Spaceship_Init(&engine->spaceship, width / 2.0f, height / 2.0f, 4.0f, 15);  // center
    for (int i = 0; i < MAX_BULLETS; i++) {
        engine->bullets[i].active = 0;
    }
}

// Update engine (spaceship movement and input)
void AstroEngine_Update(AstroEngine_t* engine) {
    // Check BTN2 (PC2)
    uint8_t current_state2 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2);
    if (current_state2 == GPIO_PIN_RESET && !button2_pressed) {  // just pressed
        Spaceship_ChangeDirection(&engine->spaceship);
        button2_pressed = 1;
    } else if (current_state2 == GPIO_PIN_SET) {  // released
        button2_pressed = 0;
    }

    // Check BTN3 (PC3) - joystick switch
    uint8_t current_state3 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3);
    if (current_state3 == GPIO_PIN_RESET && !button3_pressed) {  // just pressed
        AstroEngine_ShootBullet(engine);
        button3_pressed = 1;
    } else if (current_state3 == GPIO_PIN_SET) {  // released
        button3_pressed = 0;
    }

    // Update spaceship
    Spaceship_Update(&engine->spaceship, engine->screen_width, engine->screen_height);

    // Update bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        Bullet_Update(&engine->bullets[i], engine->screen_width, engine->screen_height);
    }
}

// Draw everything
void AstroEngine_Draw(AstroEngine_t* engine) {
    Spaceship_Draw(&engine->spaceship);
    for (int i = 0; i < MAX_BULLETS; i++) {
        Bullet_Draw(&engine->bullets[i]);
    }
}

// Shoot bullet from spaceship
void AstroEngine_ShootBullet(AstroEngine_t* engine) {
    printf("Shooting bullet!\n");
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!engine->bullets[i].active) {
            Bullet_Init(&engine->bullets[i], engine->spaceship.x, engine->spaceship.y, engine->spaceship.dx, engine->spaceship.dy, 10.0f, 4);
            break;
        }
    }
}