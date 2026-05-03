#ifndef MUSIC_H
#define MUSIC_H

#include "Buzzer.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>

extern Buzzer_cfg_t buzzer_cfg;

void playNote(int note, int duration);
void startMelody(int melody[], int durations[], int length, int bpm);
void updateMusic(void);
void stopMusic(void);

#endif
