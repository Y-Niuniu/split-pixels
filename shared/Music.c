#include "music.h"

static int current_melody[512];
static int current_durations[512];
static int current_length = 0;
static int current_index = 0;
static int wholenote = 0;
static uint32_t note_start_time = 0;
static int is_playing_melody = 0;
static int current_note_duration = 0;

void playNote(int note, int duration) {
    buzzer_tone(&buzzer_cfg, note, 50);
    current_note_duration = duration;
    is_playing_melody = 1;
    note_start_time = HAL_GetTick();
}

void startMelody(int melody[], int durations[], int length, int bpm) {
    for (int i = 0; i < length; i++) {
        current_melody[i] = melody[i];
        current_durations[i] = durations[i];
    }
    current_length = length;
    current_index = 0;
    wholenote = (60000 * 4) / bpm;//bpm=90
    
    if (length > 0) {
        current_note_duration = wholenote / current_durations[0];
        playNote(current_melody[0], current_note_duration);
    }
}

void updateMusic(void) {
    if (is_playing_melody == 0) return;
    
    if ((HAL_GetTick() - note_start_time) >= current_note_duration) {
        current_index++;
        
        if (current_index >= current_length) {
            is_playing_melody = 0;
            buzzer_off(&buzzer_cfg);
        } else {
            int duration_ms = wholenote / current_durations[current_index];
            playNote(current_melody[current_index], duration_ms);
        }
    }
}

void stopMusic(void) {
    is_playing_melody = 0;
    buzzer_off(&buzzer_cfg);
}
//you can add "int length = sizeof(melody) / sizeof(melody[0]);" in game.c to calculate the length more easier