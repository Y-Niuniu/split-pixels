#ifndef SKENGINE_H
#define SKENGINE_H
#include <stdint.h>
#include "playersetting/playersetting.h"
#include "enemysetting/enemysetting.h"
#include "bulletsetting/bulletsetting.h"
#include "weaponsetting/weaponsetting.h"
#include "wavedesign/wavedesign.h"
#include "map/map.h"
#include "Joystick.h"
#include "LCD.h"
#include "stm32l4xx_hal.h"

extern int team_score;

void SKengine_Init(void);
void SKengine_Update(void);
void SKengine_Draw(void);

#endif // SKENGINE_H