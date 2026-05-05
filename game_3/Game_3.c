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
    LCD_printString("PIXELs",  70, 70, 1, 4);
    LCD_Refresh(&cfg0);
    HAL_Delay(2000);
    
    LCD_Fill_Buffer(0);
    LCD_printString("Now that you have mastered all that you need,", 10, 30, 1, 1.7);
    LCD_printString("the time has come to face each other in battle!", 10, 50, 1, 1.4);
    LCD_printString("Only one will rise above the chaos,", 10, 70, 1, 1.4);
    LCD_printString("Only one will claim victory,", 10, 90, 1, 1.4);
    LCD_printString("And the winner, shall become the True Pixel.", 10, 110, 1, 1.4);
    LCD_Refresh(&cfg0);
    HAL_Delay(7000);
    
    // Display instructions
    LCD_Fill_Buffer(0);
    LCD_printString("Press and hold", 50, 30, 1, 2);
    LCD_printString("right btn to move", 50, 60, 1, 2);
    LCD_printString("Press left btn to shoot", 50, 70, 1, 2);
    LCD_Refresh(&cfg0);
    HAL_Delay(7000);

    printf("Astro PIXELs Game initialized.\n");

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
        
        // Check if game has ended
        if (astro_engine.game_state == GAME_STATE_END) {
            // Show end game screen
            LCD_Fill_Buffer(0);
            if (astro_engine.winner == 1) {
                LCD_printString("SPACESHIP 1 WINS!", 30, 50, 1, 3);
            } else if (astro_engine.winner == 2) {
                LCD_printString("SPACESHIP 2 WINS!", 30, 50, 1, 3);
            }
            LCD_printString("Press BTN3 to return", 35, 90, 1, 2);
            LCD_Refresh(&cfg0);
            
            // Wait for button press to return to menu
            while (1) {
                Input_Read();
                if (current_input.btn3_pressed) {
                    PWM_SetDuty(&pwm_cfg, 50);  // Reset LED
                    exit_state = MENU_STATE_HOME;
                    break;
                }
                HAL_Delay(50);
            }
            break;
        }

        // Show a brief transition screen when a map change occurs
        if (astro_engine.round_screen != ROUND_SCREEN_NONE) {
            LCD_Fill_Buffer(0);
            if (astro_engine.round_screen == ROUND_SCREEN_NEXT) {
                LCD_printString("NEXT ROUND", 20, 50, 1, 4);
            } else if (astro_engine.round_screen == ROUND_SCREEN_FINAL) {
                LCD_printString("FINAL ROUND", 20, 50, 1, 4);
            }
            LCD_printString("Get ready!", 60, 90, 1, 2);
            LCD_Refresh(&cfg0);
            HAL_Delay(1000);
            astro_engine.round_screen = ROUND_SCREEN_NONE;
        }
        
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
