#include "Game_1.h"
#include "engine.h"
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
#include "sprites/bullet_background.h"
#include "sprites/plants.h"
extern ST7789V2_cfg_t cfg0;
extern PWM_cfg_t pwm_cfg;
extern Buzzer_cfg_t buzzer_cfg;
extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;
extern MenuSystem menu; 
extern InputState current_input; 

void Engine_Init(void){}
void Engine_Update(void){}
//test for draw background sprite
void Engine_Render(void){
 LCD_Fill_Buffer(0);
//background for select plants
LCD_Draw_Sprite(77, 10, 50, 51, (const uint8_t*)TEST_wallnut);


 //background first roll of battlefield
LCD_Draw_Sprite(10, 90, 50, 42, (const uint8_t*)TEST_GRASS_FRAME0);
LCD_Draw_Sprite(52, 90, 50, 42, (const uint8_t*)TEST_GRASS_FRAME0);
LCD_Draw_Sprite(94, 90, 50, 42, (const uint8_t*)TEST_GRASS_FRAME0);
LCD_Draw_Sprite(136, 90, 50, 42, (const uint8_t*)TEST_GRASS_FRAME0);
LCD_Draw_Sprite(178, 90, 50, 42, (const uint8_t*)TEST_GRASS_FRAME0);
//background second roll of battlefield
LCD_Draw_Sprite(10, 140, 50, 42, (const uint8_t*)TEST_GRASS_FRAME0);
LCD_Draw_Sprite(52, 140, 50, 42, (const uint8_t*)TEST_GRASS_FRAME0);
LCD_Draw_Sprite(94, 140, 50, 42, (const uint8_t*)TEST_GRASS_FRAME0);
LCD_Draw_Sprite(136, 140, 50, 42, (const uint8_t*)TEST_GRASS_FRAME0);
LCD_Draw_Sprite(178, 140, 50, 42, (const uint8_t*)TEST_GRASS_FRAME0);
//background third roll of battlefield
LCD_Draw_Sprite(10, 190, 50, 42, (const uint8_t*)TEST_GRASS_FRAME0);
LCD_Draw_Sprite(52, 190, 50, 42, (const uint8_t*)TEST_GRASS_FRAME0);
LCD_Draw_Sprite(94, 190, 50, 42, (const uint8_t*)TEST_GRASS_FRAME0);
LCD_Draw_Sprite(136, 190, 50, 42, (const uint8_t*)TEST_GRASS_FRAME0);
LCD_Draw_Sprite(178, 190, 50, 42, (const uint8_t*)TEST_GRASS_FRAME0);
}
