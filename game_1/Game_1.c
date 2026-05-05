#include "Game_1.h"
#include "InputHandler.h"
#include "Menu.h"
#include "LCD.h"
#include "PWM.h"
#include "Buzzer.h"
#include "engine/interact.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>
#include "Joystick.h"
#include <stdbool.h> 
#include <math.h>
#include "shared/selector.h"
#include "sprites/bullet_background.h"
#include"engine/engine.h"
#include "melody.h"
#include "music.h"
bool IsGameOver(void);
extern ST7789V2_cfg_t cfg0;
extern PWM_cfg_t pwm_cfg;
extern Buzzer_cfg_t buzzer_cfg;
extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;
extern MenuSystem menu; 
extern InputState current_input; 
extern Joystick_t joystick_data2;
extern Joystick_cfg_t joystick_cfg2;
bool Is_nuclaunched(void);
// FSM state
typedef enum {
    INSTRUCTION,
    RUNNING,
    PAUSED,
    NULCEND
} Game1State_t;
static Game1State_t current_state = INSTRUCTION;
static int instruction_page = 0;
#define INSTRUCTION_PAGE_COUNT 9

// Exit button area
#define EXIT_X 0
#define EXIT_Y 10
#define EXIT_WIDTH 27
#define EXIT_HEIGHT 51

// Global cursor
int16_t cursor_x = 120;   
int16_t cursor_y = 120;
uint32_t last_move_tick = 0;
#define CURSOR_STEP 15         
#define MOVE_DELAY_MS 30        

static uint32_t doomshroom_timer_start = 0;

#define GAME1_FRAME_TIME_MS 30


void Game1_Init(void) {
    current_state = INSTRUCTION;
    instruction_page = 0;
    cursor_x = 120;
    cursor_y = 120;
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

static bool Game1_Update_Instruction(void) {
    static Direction last_dir = CENTRE;
    Direction dir = joystick_data.direction;
    if (dir == CENTRE) {
        dir = joystick_data2.direction;
    }
    // Either joystick can flip page
    if (dir != CENTRE && dir != last_dir) {
        instruction_page = (instruction_page + 1) % INSTRUCTION_PAGE_COUNT;
    }
    last_dir = dir;

    // Any button to skip
    if (current_input.btn2_pressed || current_input.btn3_pressed ||
        current_input.btn6_pressed || current_input.btn7_pressed ||
        current_input.btn9_pressed) {
        current_state = RUNNING;
        startMelody(BM_MELODY, BM_DURATION, BM_MELODY_LENGTH, 120);
        return true;
    }
    return true;
}

static void Game1_Render_Instruction(void) {
    LCD_Fill_Buffer(1);  // white background

    // Title changes for rules pages
    if (instruction_page < 5) {
        LCD_printString("Control guidence", 30, 5, 11, 2);
    } else {
        LCD_printString("Game Rules", 40, 5, 15, 2);
    }

    if (instruction_page == 0) {
        // P1 page 1: cursor movement
        LCD_printString("Player 1", 5, 35, 3, 3);
        LCD_printString("Use the joystick", 5, 70, 9, 2);
        LCD_printString("to move the cursor", 5, 90, 0, 2);
        LCD_printString("freely across the", 5, 110, 0, 2);
        LCD_printString("lawn battlefield", 5, 130, 0, 2);
        LCD_printString("Navigate to any", 5, 150, 0, 2);
        LCD_printString("area on the screen", 5, 170, 0, 2);
    } else if (instruction_page == 1) {
        // P1 page 2: function selection
        LCD_printString("Player 1", 5, 35, 3, 3);
        LCD_printString("Move the cursor", 5, 70, 9, 2);
        LCD_printString("over a function", 5, 90, 0, 2);
        LCD_printString("area on the screen", 5, 110, 0, 2);
        LCD_printString("Press BTN3 to", 5, 130, 4, 2);
        LCD_printString("select and confirm", 5, 150, 0, 2);
        LCD_printString("your chosen action", 5, 170, 0, 2);
    } else if (instruction_page == 2) {
        // P1 page 3: available actions
        LCD_printString("Player 1", 5, 35, 3, 3);
        LCD_printString("Place new plants", 5, 70, 0, 2);
        LCD_printString("to defend the lawn", 5, 90, 0, 2);
        LCD_printString("Can remove", 5, 110, 0, 2);
        LCD_printString("defenses or", 5, 130, 0, 2);
        LCD_printString("unwanted ones", 5, 150, 0, 2);
        LCD_printString("BTN3 pauses at exit", 5, 170, 4, 2);
    } else if (instruction_page == 3) {
        // P2 page 1: charge overview
        LCD_printString("Player 2", 5, 35, 10, 3);
        LCD_printString("At game start plants", 5, 70, 0, 2);
        LCD_printString("are fully charged", 5, 90, 0, 2);
        LCD_printString("Watch the arrows", 5, 110, 0, 2);
        LCD_printString("When color changes", 5, 130, 10, 2);
        LCD_printString("press BTN9 to", 5, 150, 7, 2);
        LCD_printString("charge for teammate", 5, 170, 0, 2);
    } else if (instruction_page == 4) {
        // P2 page 2: charge sequence
        LCD_printString("Player 2", 5, 35, 10, 3);
        LCD_printString("Use the joystick", 5, 70, 8, 2);
        LCD_printString("to light up each", 5, 90, 0, 2);
        LCD_printString("arrow in the", 5, 110, 0, 2);
        LCD_printString("correct order", 5, 130, 0, 2);
        LCD_printString("Complete the", 5, 150, 0, 2);
        LCD_printString("sequence to charge", 5, 170, 0, 2);
    } else if (instruction_page == 5) {
        // Rules page 1: starting plants
        LCD_printString("Peashooter ready", 5, 35, 3, 2);
        LCD_printString("from game start", 5, 55, 0, 2);
        LCD_printString("Fires at approaching", 5, 75, 0, 2);
        LCD_printString("zombies on the lawn", 5, 95, 0, 2);
        LCD_printString("Wall-nut ready", 5, 120, 12, 2);
        LCD_printString("from game start too", 5, 140, 0, 2);
        LCD_printString("Blocks and protects", 5, 160, 0, 2);
    } else if (instruction_page == 6) {
        // Rules page 2: cherry bomb
        LCD_printString("Cherry Bomb", 5, 35, 2, 2);
        LCD_printString("Unlocks at 600 pts", 5, 55, 0, 2);
        LCD_printString("Area explosion", 5, 75, 0, 2);
        LCD_printString("damages all nearby", 5, 95, 0, 2);
        LCD_printString("Center enemy: dies", 5, 120, 0, 2);
        LCD_printString("instantly from blast", 5, 140, 0, 2);
        LCD_printString("Surrounding: less dmg", 5, 160, 0, 2);
    } else if (instruction_page == 7) {
        // Rules page 3: doomshroom
        LCD_printString("Doomshroom", 5, 35, 13, 2);
        LCD_printString("Unlocks at", 5, 55, 0, 2);
        LCD_printString("2500 points", 5, 75, 6, 2);
        LCD_printString("Plant it to win", 5, 100, 0, 2);
        LCD_printString("the game instantly", 5, 120, 0, 2);
        LCD_printString("Ultimate weapon", 5, 140, 2, 2);
        LCD_printString("of destruction", 5, 160, 2, 2);
    } else {
        // Rules page 4: enemies
        LCD_printString("Normal Zombie:", 5, 35, 14, 2);
        LCD_printString("Fast movement", 5, 55, 0, 2);
        LCD_printString("Low hit points", 5, 75, 0, 2);
        LCD_printString("Buckethead Zombie:", 5, 100, 13, 2);
        LCD_printString("Slow movement", 5, 120, 0, 2);
        LCD_printString("High hit points", 5, 140, 0, 2);
        LCD_printString("Tough to defeat", 5, 160, 0, 2);
    }

    // Page indicator
    LCD_printString("Move joystick to flip page", 5, 190, 0, 1);

    // Skip hint
    LCD_printString("Press any button to skip", 50, 220, 0, 1);

    LCD_Refresh(&cfg0);
}

static bool Game1_Update_Running(void) {

    HandleGlobalCursor();
  if(Is_nuclaunched()){
    current_state=NULCEND;
    stopMusic();
    doomshroom_timer_start = HAL_GetTick();
    return true;
  }
    
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
        stopMusic();
        return false;
    }
}

static bool Game1_Update_Nulcend(void)
{
    LCD_Fill_Buffer(1); 
    if (HAL_GetTick() - doomshroom_timer_start >= 5000) { 
        return true; 
    }
    return false; 
}


static bool Game1_Update(void) {
    Input_Read();
    Joystick_Read(&joystick_cfg, &joystick_data);  
     Joystick_Read(&joystick_cfg2, &joystick_data2);

    switch (current_state) {
        case INSTRUCTION:
            if (!Game1_Update_Instruction()) {
                return false;
            }
            break;
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
        case NULCEND:
           
           
            if (Game1_Update_Nulcend()) {
                return false; 
            }
            
            break;
    }
   if(IsVictory()){
    stopMusic();
    LCD_Fill_Buffer(1);
    LCD_printString("VICTORY!", 80, 100, 0, 3);
    LCD_Refresh(&cfg0);
    HAL_Delay(3000);
    return false;
   }
   if(IsGameOver()){
    stopMusic();
    LCD_Fill_Buffer(1);
    LCD_printString("Zombie eat ", 15, 100, 0, 3);
    LCD_printString("your brain!!", 15, 140, 0, 3);
    LCD_Refresh(&cfg0);
    HAL_Delay(3000);
    return false;
   }


    return true;
}

static void Game1_Render(void) {
    if (current_state == INSTRUCTION) {
        Game1_Render_Instruction();
        return;
    }

    Engine_Render();
   
    
   LCD_Draw_Sprite(0, 10, 50, 26, (const uint8_t*)EXIT_BUTTON_FRAME0);
    if(current_state==NULCEND){
        LCD_Fill_Buffer(1);
        LCD_printString("BOOM!", 70, 120, 0, 4);
        LCD_Refresh(&cfg0);
        return;
    }
  
    LCD_Draw_Rect(cursor_x-2, cursor_y-2, 10, 10, 2, 1);
    
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
     Zombie_ResetGameStartTime(); 
    while (1) {
        uint32_t frame_start = HAL_GetTick();

        updateMusic();

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