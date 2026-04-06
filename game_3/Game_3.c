#include "Game_3.h"
#include "InputHandler.h"
#include "Menu.h"
#include "LCD.h"
#include "PWM.h"
#include "Buzzer.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>
#include "Spaceship/Spaceship.h" // Spaceship object
#include "AstroEngine/AstroEngine.h" // Astro game engine
#include <stdbool.h>
#include <math.h>

extern ST7789V2_cfg_t cfg0;
extern PWM_cfg_t pwm_cfg;      // LED PWM control
extern Buzzer_cfg_t buzzer_cfg; // Buzzer control

// Frame rate for this game (in milliseconds) - fastest game
#define GAME3_FRAME_TIME_MS 16  // ~60 FPS (faster than others!)

MenuState Game3_Run(void) {
    // Initialize game state
    AstroEngine_t astro_engine;
    AstroEngine_Init(&astro_engine, ST7789V2_WIDTH, ST7789V2_HEIGHT);
   
    
    // Clear screen
    LCD_Fill_Buffer(0);
    LCD_Refresh(&cfg0);

    // Startup animation
    LCD_printString("ASTRO",  70, 40, 1, 4);
    LCD_printString("PARTY",  70, 70, 1, 4);
    LCD_Refresh(&cfg0);
    HAL_Delay(1000);
    
    // Display instructions
    LCD_Fill_Buffer(0);
    LCD_printString("Press BTN2", 70, 30, 1, 2);
    LCD_printString("to change", 80, 60, 1, 2);
    LCD_printString("direction!", 75, 85, 1, 2);
    LCD_Refresh(&cfg0);
    HAL_Delay(2000);

    printf("Astro Party Game initialized.\n");

    // Play a brief startup sound
    buzzer_tone(&buzzer_cfg, 1500, 30);  // 1.5kHz at 30% volume
    HAL_Delay(50);  // Brief beep duration
    buzzer_off(&buzzer_cfg);  // Stop the buzzer
    
    MenuState exit_state = MENU_STATE_HOME;  // Default: return to menu
    
    // Game's own loop - runs until exit condition
    while (1) {
        
        uint32_t frame_start = HAL_GetTick();
        
        // Read input
        Input_Read();
        
        // Check if button was pressed to return to menu
        if (current_input.btn3_pressed) {
            PWM_SetDuty(&pwm_cfg, 50);  // Reset LED
            exit_state = MENU_STATE_HOME;
            break;  // Exit game loop
        }
        
        // UPDATE: Game logic
        AstroEngine_Update(&astro_engine);
        
        // RENDER: Clear the display buffer before drawing the game
        LCD_Fill_Buffer(0);
        AstroEngine_Draw(&astro_engine);
        LCD_Refresh(&cfg0);
        
        // Frame timing - wait for remainder of frame time
        uint32_t frame_time = HAL_GetTick() - frame_start;
        if (frame_time < GAME3_FRAME_TIME_MS) {
            HAL_Delay(GAME3_FRAME_TIME_MS - frame_time);
        }
    }
    
    return exit_state;  // Tell main where to go next
}
