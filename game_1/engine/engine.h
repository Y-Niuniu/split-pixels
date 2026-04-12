#ifndef ENGINE_H
#define ENGINE_H
#include "Menu.h"
#include "Joystick.h"
#include "Buzzer.h"
#include "PWM.h"
#include "LCD.h"
#include "data/plantdata.h"

#define b_cols 5
#define b_rows 3
#define cell_width 42
#define cell_height 50
//FSM for planting plants on battlefield
typedef enum{
    plant_none,
    plant_wallnut,
    plant_shooter,
    plant_bomb,
    plant_doomshroom,
    plant_bomb_exploded
}Planting_t;

void Engine_Init(void);
void Engine_Update(void);
void Engine_Render(void);


#endif