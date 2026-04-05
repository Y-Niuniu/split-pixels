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
#include <math.h>
#include "tools/selector.h"
#include "sprites/bullet_background.h"
#include"engine/engine.h"
extern ST7789V2_cfg_t cfg0;
extern PWM_cfg_t pwm_cfg;
extern Buzzer_cfg_t buzzer_cfg;
extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;
extern MenuSystem menu; 
extern InputState current_input; 

// FSM state
typedef enum {
    RUNNING,
    PAUSED   
} Game1State_t;
static Game1State_t current_state = RUNNING;

// Exit button area
#define EXIT_X 0
#define EXIT_Y 10
#define EXIT_WIDTH 27
#define EXIT_HEIGHT 51

// Global cursor
static int16_t cursor_x = 10;   
static int16_t cursor_y = 30;
static uint32_t last_move_tick = 0;
#define CURSOR_STEP 5          
#define MOVE_DELAY_MS 30        



#define GAME1_FRAME_TIME_MS 30

static bool IsJoystickCentered(float threshold) {
    return (fabsf(joystick_data.coord_mapped.x) < threshold &&
            fabsf(joystick_data.coord_mapped.y) < threshold);
}

void Game1_Init(void) {
   
    current_state = RUNNING;
    cursor_x = 10;
    cursor_y = 30;
    last_move_tick = 0;
    Engine_Init();

}


static void HandleGlobalCursor(void) {
    Direction dir = joystick_data.direction;
    if (dir == CENTRE) return;

    uint32_t now = HAL_GetTick();
    if ((now - last_move_tick) >= MOVE_DELAY_MS) {
        int dx = 0, dy = 0;
        switch (dir) {
            case N: dy = -CURSOR_STEP; break;
            case NE: dx = CURSOR_STEP; dy = -CURSOR_STEP; break;
            case E: dx = CURSOR_STEP; break;
            case SE: dx = CURSOR_STEP; dy = CURSOR_STEP; break;
            case S: dy = CURSOR_STEP; break;
            case SW: dx = -CURSOR_STEP; dy = CURSOR_STEP; break;
            case W: dx = -CURSOR_STEP; break;
            case NW: dx = -CURSOR_STEP; dy = -CURSOR_STEP; break;
            default: break;
        }
        cursor_x += dx;
        cursor_y += dy;
      
        if (cursor_x < 0) cursor_x = 0;
        if (cursor_x >= 240) cursor_x = 239;
        if (cursor_y < 0) cursor_y = 0;
        if (cursor_y >= 240) cursor_y = 239;
        last_move_tick = now;
    }
}

static bool Game1_Update_Running(void) {

    HandleGlobalCursor();

    
    if (current_input.btn3_pressed) {
        if (cursor_x >= EXIT_X && cursor_x <= EXIT_X + EXIT_WIDTH &&
            cursor_y >= EXIT_Y && cursor_y <= EXIT_Y + EXIT_HEIGHT) {
                current_state = PAUSED;
                return true;  
        }
    }
    Engine_Update();
    
    return true;
}

static bool Game1_Update_Paused(void) {
    select_system_t selector;
    Selector_Init(&selector);
    operation_t result = Selector_Run(&selector);
    
    if (result == RESUME) {
        current_state = RUNNING;
        return true;
    } else {
        return false; 
    }
}

static bool Game1_Update(void) {
    Input_Read();
    Joystick_Read(&joystick_cfg, &joystick_data);  

    switch (current_state) {
        case RUNNING:
            if (!Game1_Update_Running()) {
                return false;
            }
            break;
        case PAUSED:
            if (!Game1_Update_Paused()) {
                return false;
            }
            break;
    }
    return true;
}

static void Game1_Render(void) {
   
    Engine_Render();
   
    
   LCD_Draw_Sprite(0, 10, 50, 26, (const uint8_t*)EXIT_BUTTON_FRAME0);
    
  
    LCD_Draw_Rect(cursor_x-2, cursor_y-2, 4, 4, 1, 1);
    
    if (current_state == PAUSED) {
        LCD_printString("PAUSED", 70, 120, 1, 3);
        LCD_printString("resume", 40, 140, 1, 1);
        LCD_printString(" exit", 120, 140, 1, 1);
    }
    
    LCD_Refresh(&cfg0);
}

MenuState Game1_Run(void) {
    Game1_Init();
    MenuState exit_state = MENU_STATE_HOME;
    
    while (1) {
        uint32_t frame_start = HAL_GetTick();
        
        if (!Game1_Update()) {
            break;
        }
        Game1_Render();
        
        uint32_t frame_time = HAL_GetTick() - frame_start;
        if (frame_time < GAME1_FRAME_TIME_MS) {
            HAL_Delay(GAME1_FRAME_TIME_MS - frame_time);
        }
    }
    return exit_state;
}