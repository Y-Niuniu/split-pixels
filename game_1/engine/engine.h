#ifndef ENGINE_H
#define ENGINE_H
#include "Menu.h"
#include "Joystick.h"
#include "Buzzer.h"
#include "PWM.h"
#include "LCD.h"

void Engine_Init(void);
void Engine_Update(void);
void Engine_Render(void);


#endif