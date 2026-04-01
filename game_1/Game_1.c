#include "Game_1.h"
#include "InputHandler.h"
#include "Menu.h"
#include "LCD.h"
#include "PWM.h"
#include "Buzzer.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>

extern ST7789V2_cfg_t cfg0;
extern PWM_cfg_t pwm_cfg;      // LED PWM control
extern Buzzer_cfg_t buzzer_cfg; // Buzzer control
extern Joystick_cfg_t joystick_cfg // Joystick control
extern Joystick_t joystick_data

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

void game1_init(void){
    score=0;
}

void game1_update(void){
 extern MenuSystem menu;
 extern InputState current_input;
    if(current_input.btn3_pressed){
    Menu_BackToHome(&menu);
     return;
    }
}

void game1_render(void){

    extern ST7789V2_cfg_t cfg0;  // LCD configuration
    LCD_Fill_Buffer(0);  // Clear screen
    // Draw game title
    LCD_printString("GAME 1", 60, 10, 1, 3);
    char score_str[32];
    sprintf(score_str, "Score: %d", score);
    LCD_printString(score_str, 50, 100, 1, 2);
    
    // Draw instructions
    LCD_printString("Press BT3", 50, 240, 1, 1);
    LCD_printString("to return", 50, 250, 1, 1);
    
    LCD_Refresh(&cfg0);  // Update display
}
