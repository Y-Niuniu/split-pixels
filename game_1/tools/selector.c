#include "selector.h"
#include "LCD.h"
#include "InputHandler.h"
#include "Joystick.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>
#include "Game_1.h"
extern ST7789V2_cfg_t cfg0;  // LCD configuration from main.c
extern Joystick_cfg_t joystick_cfg;  // Joystick configuration
extern Joystick_t joystick_data;     // Current joystick readings
static const char* operation_options[] = {
    "Resume",
    "Exit", 
};
#define NUM_OPERATION_OPTIONS 2
#define MENU_FRAME_TIME_MS 30  

static void Game1_render_Paused_interface(select_system_t* selector){
    LCD_Fill_Buffer(0);
    LCD_printString("PAUSED", 50, 120, 1, 3);
    for (int i = 0; i < NUM_OPERATION_OPTIONS; i++) {
        uint16_t y_pos = 140 + (i * 20);
        uint8_t text_size = 1;
        
        if (i == selector->operation) {
            LCD_printString(">", 20, y_pos, 1, text_size);  
        }
        
        LCD_printString((char*)operation_options[i], 40, y_pos, 1, text_size);
    }
    LCD_Refresh(&cfg0);
}
void Selector_Init(select_system_t* selector){
    selector->operation = 0;
}
operation_t Selector_Run(select_system_t* selector) {
    static Direction last_direction = CENTRE; 
    while (1) {
        uint32_t frame_start = HAL_GetTick();
        Input_Read();
        Joystick_Read(&joystick_cfg, &joystick_data);
        Direction current_direction = joystick_data.direction;
        
        if (current_direction == S && last_direction != S) {  
            selector->operation++;
            if (selector->operation >= NUM_OPERATION_OPTIONS) {
                selector->operation = 0;  
            }
        } 
        else if (current_direction == N && last_direction != N) {  
            if (selector->operation == 0) {
                selector->operation = NUM_OPERATION_OPTIONS - 1;  
            } else {
                selector->operation--;
            }
        }
        
        last_direction = current_direction;
        
        if (current_input.btn3_pressed) {  
            if (selector->operation == 0) {
                return RESUME;
            } else {
                return EXIT;           
            }
        }
        
        Game1_render_Paused_interface(selector);
        
        uint32_t frame_time = HAL_GetTick() - frame_start;
        if (frame_time < MENU_FRAME_TIME_MS) {
            HAL_Delay(MENU_FRAME_TIME_MS - frame_time);
        }
    }
}