#include "Game_1.h"
#include "engine.h"
#include "InputHandler.h"
#include "Menu.h"
#include "LCD.h"
#include "PWM.h"
#include "Buzzer.h"
#include "stm32l4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include "Joystick.h"
#include <stdbool.h> 
#include <math.h>
#include "sprites/bullet_background.h"
#include "sprites/plants.h"
#include "sprites/arrows.h"

extern ST7789V2_cfg_t cfg0;
extern PWM_cfg_t pwm_cfg;
extern Buzzer_cfg_t buzzer_cfg;
extern Joystick_cfg_t joystick_cfg;
extern Joystick_cfg_t joystick_cfg2;
extern Joystick_t joystick_data2;
extern Joystick_t joystick_data;
extern MenuSystem menu; 
extern InputState current_input; 
extern int16_t cursor_x, cursor_y;

#define wallnut_select_x 77
#define wallnut_select_y 10
#define wallnut_select_width 50
#define wallnut_select_height 51
//define shooter selection area
#define shooter_select_x 26
#define shooter_select_y 10
#define shooter_select_width 50
#define shooter_select_height 51
//define bomb selection area
#define bomb_select_x 128
#define bomb_select_y 10
#define bomb_select_width 50   
#define bomb_select_height 51
//define doomshroom selection area
#define doomshroom_select_x 179
#define doomshroom_select_y 10
#define doomshroom_select_width 50
#define doomshroom_select_height 51
//define shovel selection area
#define shovel_select_x 0
#define shovel_select_y 60
#define shovel_select_width 26
#define shovel_select_height 30
//defien battlefield area
#define b_cols 5
#define b_rows 3
#define cell_width 50
#define cell_height 42
static const uint16_t b_x[b_cols] = {10, 52, 94, 136, 178};
static const uint16_t b_y[b_rows] = {90, 140, 190};
static int8_t sel_col = -1;
static int8_t sel_row = -1;
static bool shovel_mod = false;
/*static int8_t perv_sel_col = -1;
static int8_t perv_sel_row = -1;*/
//FSM for plant selection
typedef enum {
    select_none,
    select_wallnut,
    select_shooter,
    select_bomb,
    select_doomshroom
}PlantSelection_t;
//FSM for planting plants on battlefield
typedef enum{
    plant_none,
    plant_wallnut,
    plant_shooter,
    plant_bomb,
    plant_doomshroom
}Planting_t;
static PlantSelection_t current_selection = select_none;
static Planting_t battle_grid[b_rows][b_cols];
//FSM for wallnut charge
typedef enum {
    WALNUT_IDLE,          
    WALNUT_CHARGING,      
    WALNUT_READY,         
    WALNUT_SEQUENCE     
} WallnutState;
static WallnutState walnut_state = WALNUT_IDLE;
static uint32_t wallnut_timer_start = 0;
static uint8_t wallnut_steps = 0;
static const Direction wallnut_charge[]={S,E,S};
static const Direction shooter_charge[]={E,E,E};
static const Direction bomb_charge[]={N,S,W,N};
static const Direction doomshroom_charge[]={N,E,S,S};

static uint32_t last_charge_time = 0;
#define INPUT_DELAY_MS 200


//game engine logic
void Engine_Init(void){
current_selection = select_none;
sel_col = -1;
sel_row = -1;
/*perv_sel_col = -1;
perv_sel_row = -1;*/
for(int row=0;row<b_rows;row++){
    for(int col=0;col<b_cols;col++){
        battle_grid[row][col] = plant_none;
    }
}
}
void Engine_Update(void){
    Joystick_Read(&joystick_cfg2, &joystick_data2);
if (current_input.btn3_pressed) {
     if (cursor_x >= shovel_select_x && cursor_x <= shovel_select_x + shovel_select_width &&
        cursor_y >= shovel_select_y && cursor_y <= shovel_select_y + shovel_select_height) {
        shovel_mod = !shovel_mod;   
        return;  
    }
    if (shovel_mod) {
        shovel_mod = false;
        return;
    }
    if(current_selection!=select_none){
        current_selection = select_none;
    }
else{
    if (cursor_x >= wallnut_select_x && cursor_x <= wallnut_select_x + wallnut_select_width &&
        cursor_y >= wallnut_select_y && cursor_y <= wallnut_select_y + wallnut_select_height) {
            current_selection = select_wallnut;
    } else if (cursor_x >= shooter_select_x && cursor_x <= shooter_select_x + shooter_select_width &&
               cursor_y >= shooter_select_y && cursor_y <= shooter_select_y + shooter_select_height) {
            current_selection = select_shooter;
    } else if (cursor_x >= bomb_select_x && cursor_x <= bomb_select_x + bomb_select_width &&
               cursor_y >= bomb_select_y && cursor_y <= bomb_select_y + bomb_select_height) {
            current_selection = select_bomb;
    } else if (cursor_x >= doomshroom_select_x && cursor_x <= doomshroom_select_x + doomshroom_select_width &&
               cursor_y >= doomshroom_select_y && cursor_y <= doomshroom_select_y + doomshroom_select_height) {
            current_selection = select_doomshroom;
    }
}
}
if (current_input.btn2_pressed && shovel_mod) {
    if (sel_col != -1 && sel_row != -1 && battle_grid[sel_row][sel_col] != plant_none) {
        battle_grid[sel_row][sel_col] = plant_none;   
        shovel_mod = false;                          
    }
} 
else{
switch (current_selection) {
    case select_wallnut:
      if (current_input.btn2_pressed && sel_col != -1 && sel_row != -1 &&
        battle_grid[sel_row][sel_col] == plant_none &&
        walnut_state == WALNUT_IDLE) {  
        battle_grid[sel_row][sel_col] = plant_wallnut;
        current_selection = select_none;
        walnut_state = WALNUT_CHARGING;
        wallnut_timer_start = HAL_GetTick();
    }
    break;

      
    case select_shooter:
        // Handle shooter selection logic
        break;
    case select_bomb:
        // Handle bomb selection logic
        break;
    case select_doomshroom:
        // Handle doomshroom selection logic
        break;
    case select_none:
        // No selection or handle default case
        break;
}
}
switch (walnut_state) {
    case WALNUT_CHARGING:
        if ((HAL_GetTick() - wallnut_timer_start) >= 10000) {
            walnut_state = WALNUT_READY;
        }
        break;
    case WALNUT_READY:
        if (current_input.btn9_pressed) {   
            walnut_state = WALNUT_SEQUENCE;
            wallnut_steps = 0;
            last_charge_time = HAL_GetTick();
        }
        break;
    case WALNUT_SEQUENCE:
       
        Direction dir = joystick_data2.direction;
        if (dir != CENTRE && (HAL_GetTick() - last_charge_time) >= INPUT_DELAY_MS) {
            if (dir == wallnut_charge[wallnut_steps]) {
                wallnut_steps++;
                if (wallnut_steps >= sizeof(wallnut_charge)/sizeof(wallnut_charge[0])) {
                
                    if (sel_col != -1 && sel_row != -1 && battle_grid[sel_row][sel_col] == plant_none) {
                        battle_grid[sel_row][sel_col] = plant_wallnut;
                    }
                    walnut_state = WALNUT_IDLE;   
                }
            } else {
                wallnut_steps = 0;  
            }
            last_charge_time = HAL_GetTick();
        }
        break;
    default:
        break;
}
if (current_selection != select_none) {
    sel_col = -1;
    sel_row = -1;
    for (int row = 0; row < b_rows; row++) {
        for (int col = 0; col < b_cols; col++) {
            if (cursor_x >= b_x[col] && cursor_x <= b_x[col] + cell_width &&
                cursor_y >= b_y[row] && cursor_y <= b_y[row] + cell_height) {
                sel_col = col;
                sel_row = row;
                break;
            }
        }
        if (sel_col != -1) break;
    }
}
/*if(sel_col!=perv_sel_col||sel_row!=perv_sel_row){
if(perv_sel_col!=-1){
    LCD_Draw_Sprite(b_x[perv_sel_col], b_y[perv_sel_row], cell_width, cell_height, (const uint8_t*)TEST_GRASS_FRAME0);
 }
 if(sel_col!=-1){
     LCD_Draw_Sprite(b_x[sel_col], b_y[sel_row], cell_width, cell_height, (const uint8_t*)TEST_GRASS_FRAME1);
 }
 perv_sel_col = sel_col;
 perv_sel_row = sel_row;
 }*/
}

//test for draw background sprite
void Engine_Render(void){
 LCD_Fill_Buffer(0);
//background for select plants

LCD_Draw_Sprite(77, 10, 50, 51, (const uint8_t*)TEST_select_walnut);
LCD_Draw_Sprite(26, 10, 50, 51, (const uint8_t*)TEST_select_shooter);
LCD_Draw_Sprite(128, 10, 50, 51, (const uint8_t*)TEST_select_bomb);
LCD_Draw_Sprite(179, 10, 50, 51, (const uint8_t*)TEST_select_doomshroom);
LCD_Draw_Sprite(26, 60, 30, 51, (const uint8_t *) ARROW_SHOOTER_FRAMES[0]);
LCD_Draw_Sprite(128, 60, 30, 51, (const uint8_t *) ARROW_BOMB_FRAMES[0]);
LCD_Draw_Sprite(179, 60, 30, 51, (const uint8_t *) ARROW_DOOMSHROOM_FRAMES[0]);
LCD_Draw_Sprite(220, 10, 80, 20, (const uint8_t *)TEST_GENERATE); 
LCD_Draw_Sprite(220, 90, 50, 20, (const uint8_t *)TEST_GENERATE); 
LCD_Draw_Sprite(220, 140, 50, 20, (const uint8_t *)TEST_GENERATE); 
LCD_Draw_Sprite(220, 190, 50, 20, (const uint8_t *)TEST_GENERATE); 
LCD_Draw_Sprite(0, 90, 50, 10, (const uint8_t *)TEST_faildetect); 
LCD_Draw_Sprite(0, 140, 50, 10, (const uint8_t *)TEST_faildetect); 
LCD_Draw_Sprite(0, 190, 50, 10, (const uint8_t *)TEST_faildetect); 
LCD_Draw_Sprite(0, 60, 30, 26, (const uint8_t *)TEST_SHOVEL);
 if(shovel_mod)  {
    LCD_Draw_Rect(0, 60, 30, 26, 2, 0);
 }

switch (walnut_state) {
        case WALNUT_IDLE:
            LCD_Draw_Sprite(77, 60, 30, 51, (const uint8_t*)ARROW_WALLNUT_FRAMES[3]);
            break;
        case WALNUT_CHARGING:
            LCD_Draw_Sprite(77, 60, 30, 51, (const uint8_t*)ARROW_WALLNUT_FRAMES[0]);
            break;
        case WALNUT_READY:
            LCD_Draw_Sprite(77, 60, 30, 51, (const uint8_t*)ARROW_WALLNUT_FRAMES[ARROW_WALLNUT_FRAME_COUNT - 1]);
            break;
        case WALNUT_SEQUENCE:
            LCD_Draw_Sprite(77, 60, 30, 51, ARROW_WALLNUT_FRAMES[wallnut_steps]);
            break;
    }

for (int row = 0; row < b_rows; row++) {
        for (int col = 0; col < b_cols; col++) {
            const uint8_t* sprite = NULL;
            switch(battle_grid[row][col]){
                case plant_wallnut:
                    sprite = (const uint8_t*)TEST_wallnut;
               break;
        
                case plant_none:
           default:
            if (col == sel_col && row == sel_row&&current_selection!=select_none) {
                sprite = (const uint8_t*)TEST_GRASS_FRAME1;
            } else {
                sprite = (const uint8_t*)TEST_GRASS_FRAME0;
            } 
            break;
            }
            LCD_Draw_Sprite(b_x[col], b_y[row], cell_width, cell_height, sprite);
        
    }
}
}