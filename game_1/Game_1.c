#include "Game_1.h"
#include "InputHandler.h"
#include "Menu.h"
#include "LCD.h"
#include "PWM.h"
#include "Buzzer.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>
#include "Joystick.h"
#include <stdbool.h> 
extern ST7789V2_cfg_t cfg0;
extern PWM_cfg_t pwm_cfg;      // LED PWM control
extern Buzzer_cfg_t buzzer_cfg; // Buzzer control
extern Joystick_cfg_t joystick_cfg; // Joystick control
extern Joystick_t joystick_data;

/**
 * @brief Game 1 Implementation - Student can modify
 * 
 * EXAMPLE: Shows how to use PWM LED for visual feedback
 * This is a placeholder with a bouncing animation that changes LED brightness.
 * Replace this with your actual game logic!
 */

// Game state - customize for your game
static uint32_t animation_counter = 0;
static int16_t moving_x = 0;
static int8_t move_direction = 1;

// Frame rate for this game (in milliseconds)
#define GAME1_FRAME_TIME_MS 30  // ~33 FPS

/**
 * @brief Initialize Game 1
 * Called once when the game starts
 */
static void Game1_Init(void) {
    animation_counter = 0;
    moving_x = 0;
    move_direction = 1;
    
    // Play a brief startup sound
    buzzer_tone(&buzzer_cfg, 1000, 30);  // 1kHz at 30% volume
    HAL_Delay(50);  // Brief beep duration
    buzzer_off(&buzzer_cfg);  // Stop the buzzer
}

/**
 * @brief Update Game 1 logic
 * Called once per frame. Returns true to continue game, false to exit.
 */
static bool Game1_Update(void) {
    // Read input
    Input_Read();
    
    // Check if button was pressed to return to menu
    if (current_input.btn3_pressed) {
        PWM_SetDuty(&pwm_cfg, 50);  // Reset LED to 50% when returning
        return false;  // Exit game
    }
    
    // UPDATE: Game logic
    animation_counter++;
    
    // Simple animation: move object back and forth
    moving_x += move_direction * 3;
    if (moving_x >= 200 || moving_x <= 0) {
        move_direction *= -1;
    }
    
    // Example: Vary LED brightness based on animation
    uint8_t brightness = (moving_x * 100) / 200;
    PWM_SetDuty(&pwm_cfg, brightness);
    
    return true;  // Continue game
}

/**
 * @brief Render Game 1
 * Called once per frame after Update()
 */
static void Game1_Render(void) {
    // RENDER: Draw to LCD
    LCD_Fill_Buffer(0);
    
    // Title
    LCD_printString("GAME 1", 60, 10, 1, 3);
    
    // Simple animated object (moving box)
    LCD_printString("[*]", 20 + moving_x, 100, 1, 3);
    
    // Display counter
    char counter[32];
    sprintf(counter, "Frame: %lu", animation_counter);
    LCD_printString(counter, 50, 150, 1, 2);
    
    // Show PWM LED usage
    LCD_printString("LED: PWM Demo", 30, 180, 1, 1);
    char pwm_str[32];
    sprintf(pwm_str, "Brightness: %d%%", (moving_x * 100) / 200);
    LCD_printString(pwm_str, 30, 195, 1, 1);
    
    // Instructions
    LCD_printString("Press BT3 to", 40, 220, 1, 1);
    LCD_printString("Return to Menu", 40, 235, 1, 1);
    
    LCD_Refresh(&cfg0);
}

MenuState Game1_Run(void) {
    // Initialize game
    Game1_Init();
    
    MenuState exit_state = MENU_STATE_HOME;  // Default: return to menu
    
    // Game's own loop - runs until exit condition
    while (1) {
        uint32_t frame_start = HAL_GetTick();
        
        // Update game logic
        if (!Game1_Update()) {
            break;  // Exit game loop
        }
        
        // Render game
        Game1_Render();
        
        // Frame timing - wait for remainder of frame time
        uint32_t frame_time = HAL_GetTick() - frame_start;
        if (frame_time < GAME1_FRAME_TIME_MS) {
            HAL_Delay(GAME1_FRAME_TIME_MS - frame_time);
        }
    }
    
    return exit_state;  // Tell main where to go next
}
