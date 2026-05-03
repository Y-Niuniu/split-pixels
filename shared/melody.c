#include "melody.h"

#define NOTE_C3  131
#define NOTE_D3  147
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_G3  196
#define NOTE_A3  220
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_REST 0

int melody[] = {
        NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_D4, NOTE_D4,
        NOTE_REST,
        NOTE_C4, NOTE_C4, NOTE_C4, NOTE_C4, NOTE_B3, NOTE_G3, NOTE_E3,
        NOTE_REST,
        NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_D4, NOTE_D4,
        NOTE_REST,
        NOTE_C4, NOTE_C4, NOTE_C4, NOTE_C4, NOTE_B3, NOTE_G3, NOTE_E3,

        NOTE_A3,
        NOTE_REST,
        NOTE_B3, NOTE_G3, NOTE_E3, NOTE_A3,
        NOTE_REST,
        NOTE_C4, NOTE_B3, NOTE_A3, NOTE_B3, NOTE_C4, NOTE_D4,
        NOTE_C4, NOTE_B3, NOTE_C4,

        NOTE_B3, NOTE_A3, NOTE_B3, NOTE_A3, NOTE_G3, NOTE_A3,
        NOTE_B3, NOTE_C4, NOTE_F4, NOTE_E4, NOTE_F4,
        NOTE_E4, NOTE_G4, NOTE_D4
};
int durations[] = {
        8, 8, 8, 8, 8, 4,
        8,
        8, 8, 8, 8, 8, 8, 8,
        8,
        8, 8, 8, 8, 8, 4,
        8,
        8, 8, 8, 8, 8, 8, 8,

        2,
        8,
        8, 8, 8, 2,
        8,
        8, 8, 8, 8, 8, 4,
        8, 8, 4,

        8, 8, 8, 8, 8, 4,
        8, 8, 8, 8, 4,
        4, 8, 4
};
