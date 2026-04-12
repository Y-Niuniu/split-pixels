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
#include"interact.h"
#include"data/zombiedata.h"
#include"data/plantdata.h"

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
bool Is_nuclaunched(void);
//wallnut selection area
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

//determine battlefield cell positions
const uint16_t b_x[b_cols] = {10, 52, 94, 136, 178};
const uint16_t b_y[b_rows] = {90, 140, 190};
static uint32_t bomb_planted_time[b_rows][b_cols];
static int8_t sel_col = -1;
static int8_t sel_row = -1;
static bool shovel_mod = false;
uint16_t plant_health[b_rows][b_cols];
//FSM for plant selection
typedef enum {
    select_none,
    select_wallnut,
    select_shooter,
    select_bomb,
    select_doomshroom
}PlantSelection_t;
static PlantSelection_t current_selection = select_none;
Planting_t battle_grid[b_rows][b_cols];
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
//FSM for shooter charge 
typedef enum {
    SHOOTER_IDLE,          
    SHOOTER_CHARGING,      
    SHOOTER_READY,         
    SHOOTER_SEQUENCE     
} ShooterState;
static ShooterState shooter_state = SHOOTER_IDLE;
static uint32_t shooter_timer_start = 0;
static uint8_t shooter_steps = 0;
//FSM for bomb charge
typedef enum {
    BOMB_IDLE,          
    BOMB_CHARGING,      
    BOMB_READY,         
    BOMB_SEQUENCE     
} BombState;
static BombState bomb_state = BOMB_IDLE;
static uint32_t bomb_timer_start = 0;
static uint8_t bomb_steps = 0;
//bomb explosion logic
static void Explode(int row, int col){
   
    for(int i=0; i<MAX_ZOMBIES; i++){
        if(zombie_instances[i].is_alive && zombie_instances[i].row == row){
            int z_col = (zombie_instances[i].x - b_x[0]) / cell_width;
            if(z_col == col){
                zombie_instances[i].is_alive = false;
                Score_Add(zombie_status[zombie_instances[i].type].score);
            }
        }
    }

    int adjacent_dirs[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
    for(int d=0; d<4; d++){
        int new_row = row + adjacent_dirs[d][0];
        int new_col = col + adjacent_dirs[d][1];
        if (new_row >= 0 && new_row < b_rows && new_col >= 0 && new_col < b_cols) {
            for (int i = 0; i < MAX_ZOMBIES; i++) {
                if (zombie_instances[i].is_alive && zombie_instances[i].row == new_row) {
                    int z_col = (zombie_instances[i].x - b_x[0]) / cell_width;
                    if (z_col == new_col) {
                        zombie_instances[i].health /= 2; // 50% damage
                        if (zombie_instances[i].health <= 0) {
                            zombie_instances[i].is_alive = false;
                            Score_Add(zombie_status[zombie_instances[i].type].score);
                        }
                    }
                }
            }
        }
    }

   
    int diagonal_dirs[4][2] = {{-1,-1}, {-1,1}, {1,-1}, {1,1}};
    for(int d=0; d<4; d++){
        int new_row = row + diagonal_dirs[d][0];
        int new_col = col + diagonal_dirs[d][1];
        if (new_row >= 0 && new_row < b_rows && new_col >= 0 && new_col < b_cols) {
            for (int i = 0; i < MAX_ZOMBIES; i++) {
                if (zombie_instances[i].is_alive && zombie_instances[i].row == new_row) {
                    int z_col = (zombie_instances[i].x - b_x[0]) / cell_width;
                    if (z_col == new_col) {
                        zombie_instances[i].health -= zombie_instances[i].health / 4; // 25% damage
                        if (zombie_instances[i].health <= 0) {
                            zombie_instances[i].is_alive = false;
                            Score_Add(zombie_status[zombie_instances[i].type].score);
                        }
                    }
                }
            }
        }
    }
}





//FSM for doomshroom 
typedef enum {
    DOOMSHROOM_INACTIVE,   
    DOOMSHROOM_CHARGEABLE, 
    DOOMSHROOM_CHARGING,    
    DOOMSHROOM_READY,       
    DOOMSHROOM_PLANTED      
} DoomshroomState;
static DoomshroomState doomshroom_state = DOOMSHROOM_INACTIVE;
static uint8_t doomshroom_steps = 0;
static uint32_t doomshroom_timer_start = 0;
static bool nuclaunch=false;



static const Direction wallnut_charge[]={S,E,S};
static const Direction shooter_charge[]={E,E,E};
static const Direction bomb_charge[]={N,S,W,N};
static const Direction doomshroom_charge[]={N,E,S,S,S};

static uint32_t wallnut_last_charge_time = 0;
static uint32_t shooter_last_charge_time = 0;
static uint32_t bomb_last_charge_time = 0;
static uint32_t doomshroom_last_charge_time = 0;

#define INPUT_DELAY_MS 200
bool Is_nuclaunched(void){
    return nuclaunch;// if true return to main menu
}

//game engine logic
void Engine_Init(void){
current_selection = select_none;
sel_col = -1;
sel_row = -1;
nuclaunch=false;
doomshroom_state = DOOMSHROOM_INACTIVE;
doomshroom_timer_start = HAL_GetTick(); 
//make sure all cells are empty
for(int row=0;row<b_rows;row++){
    for(int col=0;col<b_cols;col++){
        battle_grid[row][col] = plant_none;
        bomb_planted_time[row][col]=0;
        plant_health[row][col]=0;
    }
}
Zombie_Init();
}
void Engine_Update(void){
static uint32_t last_damage_time=0;
static uint32_t last_frame_time=0;
uint32_t now=HAL_GetTick();

uint32_t delta =now-last_frame_time;
if(delta>=100)delta=100;
last_frame_time=now;

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
        current_selection = select_none;// first decide is shovel mod then select plant
    }
else{
    if (cursor_x >= wallnut_select_x && cursor_x <= wallnut_select_x + wallnut_select_width &&
        cursor_y >= wallnut_select_y && cursor_y <= wallnut_select_y + wallnut_select_height) {
            current_selection = select_wallnut;
    } else if (cursor_x >= shooter_select_x && cursor_x <= shooter_select_x + shooter_select_width &&
               cursor_y >= shooter_select_y && cursor_y <= shooter_select_y + shooter_select_height) {
            current_selection = select_shooter;
    } else if (cursor_x >= bomb_select_x && cursor_x <= bomb_select_x + bomb_select_width &&
               cursor_y >= bomb_select_y && cursor_y <= bomb_select_y + bomb_select_height && Score_Get() >= 600) {
            current_selection = select_bomb;
    } else if (cursor_x >= doomshroom_select_x && cursor_x <= doomshroom_select_x + doomshroom_select_width &&
               cursor_y >= doomshroom_select_y && cursor_y <= doomshroom_select_y + doomshroom_select_height &&
							 doomshroom_state == DOOMSHROOM_READY && Score_Get() >= 2500) { 
            current_selection = select_doomshroom;
    }
}
}
if (current_input.btn2_pressed && shovel_mod) {
    if (sel_col != -1 && sel_row != -1 && battle_grid[sel_row][sel_col] != plant_none) {
        Planting_t remove_plant = battle_grid[sel_row][sel_col];
        battle_grid[sel_row][sel_col] = plant_none;   
        switch (remove_plant) {
            case plant_wallnut:
                //walnut_state = WALNUT_IDLE;
                break;
            case plant_shooter:
                //shooter_state = SHOOTER_IDLE;
                break;
            case plant_bomb:
                //bomb_state = BOMB_IDLE;
                break;
            case plant_doomshroom:
                doomshroom_state = DOOMSHROOM_INACTIVE;
                doomshroom_timer_start = HAL_GetTick();
                break;
            case plant_none:
            case plant_bomb_exploded:
                // Do nothing for these cases
                break;
        }
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
        plant_health[sel_row][sel_col]=600;
        current_selection = select_none;
        walnut_state = WALNUT_CHARGING;
        wallnut_timer_start = HAL_GetTick();
    }
    break;
    case select_shooter:
        if (current_input.btn2_pressed && sel_col != -1 && sel_row != -1 &&
        battle_grid[sel_row][sel_col] == plant_none &&
        shooter_state == SHOOTER_IDLE) {  
        battle_grid[sel_row][sel_col] = plant_shooter;
        plant_health[sel_row][sel_col]=125;
        current_selection = select_none;
        shooter_state = SHOOTER_CHARGING;
        shooter_timer_start = HAL_GetTick();
    }
        break;
    case select_bomb:
        if (current_input.btn2_pressed && sel_col != -1 && sel_row != -1 &&
        battle_grid[sel_row][sel_col] == plant_none &&
        bomb_state == BOMB_IDLE) {  
        battle_grid[sel_row][sel_col] = plant_bomb;
        bomb_planted_time[sel_row][sel_col]=HAL_GetTick();
        current_selection = select_none;
        bomb_state = BOMB_CHARGING;
        bomb_timer_start = HAL_GetTick();
    }
        break;
    case select_doomshroom:
        
        if (current_input.btn2_pressed && sel_col != -1 && sel_row != -1 &&
        battle_grid[sel_row][sel_col] == plant_none) {  
        battle_grid[sel_row][sel_col] = plant_doomshroom;
        plant_health[sel_row][sel_col]=1;
        nuclaunch=true; 
        doomshroom_state = DOOMSHROOM_PLANTED; 
				current_selection = select_none; 
    }
        break;
    case select_none:
        // No selection or handle default case
        break;
}
}
Zombie_Update(delta);
if(now-last_damage_time>=1000){
last_damage_time=now;
Damage_Determination();
}


//press btn9 to charge only one plant to avoid bugs
if(current_input.btn9_pressed){
   if(shooter_state == SHOOTER_READY){
        shooter_state = SHOOTER_SEQUENCE;
        shooter_steps = 0;
        shooter_last_charge_time = HAL_GetTick();
    }
    else if(walnut_state == WALNUT_READY){
        walnut_state = WALNUT_SEQUENCE;
        wallnut_steps = 0;
        wallnut_last_charge_time = HAL_GetTick();
    }
    else if (bomb_state == BOMB_READY) {
        bomb_state = BOMB_SEQUENCE;
        bomb_steps = 0;
        bomb_last_charge_time = HAL_GetTick();
    }else if( doomshroom_state == DOOMSHROOM_CHARGEABLE){
        doomshroom_state = DOOMSHROOM_CHARGING;
        doomshroom_steps = 0;
        doomshroom_last_charge_time = HAL_GetTick();
    }
}

switch (walnut_state) {
    case WALNUT_CHARGING:
        if ((HAL_GetTick() - wallnut_timer_start) >= 10000) {
            walnut_state = WALNUT_READY;
        }
        break;
    case WALNUT_READY:
       
        break;
    case WALNUT_SEQUENCE:
       
        Direction dir = joystick_data2.direction;
        if (dir != CENTRE && (HAL_GetTick() - wallnut_last_charge_time) >= INPUT_DELAY_MS) {
            if (dir == wallnut_charge[wallnut_steps]) {
                wallnut_steps++;
                if (wallnut_steps >= sizeof(wallnut_charge)/sizeof(wallnut_charge[0])) {
                    
                    walnut_state = WALNUT_IDLE;   
                }
            } else {
                wallnut_steps = 0;  
            }
            wallnut_last_charge_time = HAL_GetTick();
        }
        break;
    default:
        break;
}
switch (shooter_state) {
    case SHOOTER_CHARGING:
        if ((HAL_GetTick() - shooter_timer_start) >= 6500) {
            shooter_state = SHOOTER_READY;
        }
        break;
    case SHOOTER_READY:
      
        break;
    case SHOOTER_SEQUENCE:
       
        Direction dir = joystick_data2.direction;
        if (dir != CENTRE && (HAL_GetTick() - shooter_last_charge_time) >= INPUT_DELAY_MS) {
            if (dir == shooter_charge[shooter_steps]) {
                shooter_steps++;
                if (shooter_steps >= sizeof(shooter_charge)/sizeof(shooter_charge[0])) {
                
                   
                    shooter_state = SHOOTER_IDLE;   
                }
            } else {
                shooter_steps = 0;  
            }
           shooter_last_charge_time = HAL_GetTick();
        }
        break;
    default:
        break;
}

switch (bomb_state) {
    case BOMB_CHARGING:
        if ((HAL_GetTick() - bomb_timer_start) >= 15000) {
            bomb_state = BOMB_READY;
        }
        break;
    case BOMB_READY:
      
        break;
    case BOMB_SEQUENCE:
       
        Direction dir = joystick_data2.direction;
        if (dir != CENTRE && (HAL_GetTick() - bomb_last_charge_time) >= INPUT_DELAY_MS) {
            if (dir == bomb_charge[bomb_steps]) {
                bomb_steps++;
                if (bomb_steps >= sizeof(bomb_charge)/sizeof(bomb_charge[0])) {
                   
                    bomb_state = BOMB_IDLE;   
                }
            } else {
                bomb_steps = 0;  
            }
           bomb_last_charge_time = HAL_GetTick();
        }
        break;
    default:
        break;
}
switch (doomshroom_state) {
    case DOOMSHROOM_INACTIVE:
        
        if (HAL_GetTick() - doomshroom_timer_start >= 120000) {
            doomshroom_state = DOOMSHROOM_CHARGEABLE;
        }
        break;
    case DOOMSHROOM_CHARGEABLE:
       
        break;
    case DOOMSHROOM_CHARGING:
        
        Direction dir = joystick_data2.direction;
        if (dir != CENTRE && (HAL_GetTick() - doomshroom_last_charge_time) >= INPUT_DELAY_MS) {
            if (dir == doomshroom_charge[doomshroom_steps]) {
                doomshroom_steps++;
               
                if (doomshroom_steps >= sizeof(doomshroom_charge)/sizeof(doomshroom_charge[0])) {
                    doomshroom_state = DOOMSHROOM_READY; 
                }
            } else {
               
                doomshroom_steps = 0;  
            }
            doomshroom_last_charge_time = HAL_GetTick();
        }
        break;
    case DOOMSHROOM_READY:
   
        break;
    case DOOMSHROOM_PLANTED:
    
        break;
}

for (int row = 0; row < b_rows; row++) {
    for (int col = 0; col < b_cols; col++) {
        if (battle_grid[row][col] == plant_bomb && bomb_planted_time[row][col] != 0) {
            if (HAL_GetTick() - bomb_planted_time[row][col] > 1500) { 
                Explode(row, col);
                battle_grid[row][col] = plant_bomb_exploded;
                bomb_planted_time[row][col] = HAL_GetTick(); // Reset timer for removal animation
            }
        } else if (battle_grid[row][col] == plant_bomb_exploded) {
            if (HAL_GetTick() - bomb_planted_time[row][col] > 200) {
                battle_grid[row][col] = plant_none;
                bomb_planted_time[row][col] = 0;
            }
        }
    }
}

if (current_selection != select_none||shovel_mod) {
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

char score_str[20];
sprintf(score_str, "Score: %d", Score_Get());
LCD_printString(score_str,0,0,1,1);

//background for select plants

LCD_Draw_Sprite(77, 10, 50, 51, (const uint8_t*)TEST_select_walnut);
LCD_Draw_Sprite(26, 10, 50, 51, (const uint8_t*)TEST_select_shooter);
LCD_Draw_Sprite(128, 10, 50, 51, (const uint8_t*)TEST_select_bomb);
LCD_Draw_Sprite(179, 10, 50, 71, (const uint8_t*)TEST_select_doomshroom);
LCD_Draw_Sprite(220, 90, 50, 20, (const uint8_t *)TEST_GENERATE); 
LCD_Draw_Sprite(220, 140, 50, 20, (const uint8_t *)TEST_GENERATE); 
LCD_Draw_Sprite(220, 190, 50, 20, (const uint8_t *)TEST_GENERATE); 
LCD_Draw_Sprite(0, 90, 50, 10, (const uint8_t *)TEST_faildetect); 
LCD_Draw_Sprite(0, 140, 50, 10, (const uint8_t *)TEST_faildetect); 
LCD_Draw_Sprite(0, 190, 50, 10, (const uint8_t *)TEST_faildetect); 
LCD_Draw_Sprite(0, 60, 30, 26, (const uint8_t *)TEST_SHOVEL);
 if(shovel_mod)  {
    LCD_Draw_Rect(0, 60, 26, 30, 2, 0);
 }
if(select_wallnut){
 LCD_Draw_Rect(77, 10, 51, 50, 2, 0);
}
if(select_shooter){
 LCD_Draw_Rect(26, 10, 51, 50, 2, 0);
}
if(select_bomb){
 LCD_Draw_Rect(128, 10, 51, 50, 2, 0);
}

switch (walnut_state) {
        case WALNUT_IDLE:
            LCD_Draw_Sprite(77, 60, 30, 51, (const uint8_t*)ARROW_WALLNUT_FRAMES[3]);
            break;
        case WALNUT_CHARGING:
            LCD_Draw_Sprite(77, 60, 30, 51, (const uint8_t*)ARROW_WALLNUT_FRAMES[0]);
            break;
        case WALNUT_READY:
            LCD_Draw_Sprite(77, 60, 30, 51, (const uint8_t*)ARROW_WALLNUT_FRAMES[4]);
            break;
        case WALNUT_SEQUENCE:
            LCD_Draw_Sprite(77, 60, 30, 51, ARROW_WALLNUT_FRAMES[wallnut_steps]);
            break;
    }
switch (shooter_state) {
        case SHOOTER_IDLE:
            LCD_Draw_Sprite(26, 60, 30, 51, (const uint8_t*)ARROW_SHOOTER_FRAMES[3]);
            break;
        case SHOOTER_CHARGING:
            LCD_Draw_Sprite(26, 60, 30, 51, (const uint8_t*)ARROW_SHOOTER_FRAMES[0]);
            break;
        case SHOOTER_READY:
            LCD_Draw_Sprite(26, 60, 30, 51, (const uint8_t*)ARROW_SHOOTER_FRAMES[4]);
            break;
        case SHOOTER_SEQUENCE:
            LCD_Draw_Sprite(26, 60, 30, 51, ARROW_SHOOTER_FRAMES[shooter_steps]);
            break;
    }
    switch (bomb_state) {
        case BOMB_IDLE:
            LCD_Draw_Sprite(128, 60, 30, 51, (const uint8_t*)ARROW_BOMB_FRAMES[4]);
            break;
        case BOMB_CHARGING:
            LCD_Draw_Sprite(128, 60, 30, 51, (const uint8_t*)ARROW_BOMB_FRAMES[0]);
            break;
        case BOMB_READY:
            LCD_Draw_Sprite(128, 60, 30, 51, (const uint8_t*)ARROW_BOMB_FRAMES[5]);
            break;
        case BOMB_SEQUENCE:
            LCD_Draw_Sprite(128, 60, 30, 51, ARROW_BOMB_FRAMES[bomb_steps]);
            break;
    }
switch (doomshroom_state) {
        case DOOMSHROOM_INACTIVE:
        case DOOMSHROOM_PLANTED:
            LCD_Draw_Sprite(179, 60, 30, 71, (const uint8_t*)ARROW_DOOMSHROOM_FRAMES[0]);
            break;
        case DOOMSHROOM_CHARGEABLE:
            LCD_Draw_Sprite(179, 60, 30, 71, (const uint8_t*)ARROW_DOOMSHROOM_FRAMES[5]);
            break;
        case DOOMSHROOM_CHARGING:
            LCD_Draw_Sprite(179, 60, 30, 71, ARROW_DOOMSHROOM_FRAMES[doomshroom_steps]);
            break;
        case DOOMSHROOM_READY:
            LCD_Draw_Sprite(179, 60, 30, 71, (const uint8_t*)ARROW_DOOMSHROOM_FRAMES[5]);
            break;
    }

for (int row = 0; row < b_rows; row++) {
        for (int col = 0; col < b_cols; col++) {
            const uint8_t* sprite = NULL;
            switch(battle_grid[row][col]){
                case plant_wallnut:
                    sprite = (const uint8_t*)TEST_wallnut;
               break;
                case plant_shooter:
                    sprite = (const uint8_t*)TEST_shooter;
                    break;
                case plant_bomb:
                    sprite = (const uint8_t*) TEST_BOMB_FRAMES[0];
                    break;
                case plant_bomb_exploded:
                    sprite = (const uint8_t*) TEST_BOMB_FRAMES[1];
                    break;
                case plant_doomshroom:
                    sprite = (const uint8_t*)TEST_doomshroom;
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
            LCD_Draw_Sprite(b_x[col], b_y[row], cell_height, cell_width, sprite);
        
    }
}
Zombie_Render();
}