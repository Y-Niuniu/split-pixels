#include "Game_2.h"
#include "InputHandler.h"
#include "Menu.h"
#include "LCD.h"
#include "Buzzer.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>
#include "SKengine/SKengine.h"

extern ST7789V2_cfg_t cfg0;
extern Buzzer_cfg_t buzzer_cfg;  // Buzzer control

/**
 * @brief Game 2 Implementation - Student can modify
 * 
 * EXAMPLE: Shows how to use the Buzzer for sound effects
 * This is a placeholder with a bouncing animation.
 * Replace this with your actual game logic!
 */

// Game state - customize for your game
//static uint32_t animation_counter = 0;
//static int16_t moving_y = 0;
//static int8_t move_direction = 1;

// Frame rate for this game (in milliseconds) - runs slower than Game 1
#define GAME2_FRAME_TIME_MS 50  // ~20 FPS (different from Game 1!)

MenuState Game2_Run(void) {
    SKengine_Init();

    MenuState exit_state = MENU_STATE_HOME;

    while (1) {
        uint32_t frame_start = HAL_GetTick();

        Input_Read();

        if (current_input.btn3_pressed) {
            exit_state = MENU_STATE_HOME;
            break;
        }

        SKengine_Update();
        SKengine_Draw();

        LCD_Refresh(&cfg0);

        uint32_t frame_time = HAL_GetTick() - frame_start;
        if (frame_time < GAME2_FRAME_TIME_MS) {
            HAL_Delay(GAME2_FRAME_TIME_MS - frame_time);
        }
    }

    return exit_state;
}