#include "music.h"

static int current_melody[512];  //maximun melody length
static int current_durations[512];  //node durations
static int current_length = 0;  
static int current_index = 0;
static int wholenote = 0;
static uint32_t note_start_time = 0;
static int is_playing_melody = 0;
static int current_note_duration = 0;

void playNote(int note, int duration) {  //for play one node
    buzzer_tone(&buzzer_cfg, note, 50);
    current_note_duration = duration;
    is_playing_melody = 1;
    note_start_time = HAL_GetTick();
}
//melody and durations should be same length

void startMelody(int melody[], int durations[], int length, int bpm) { //for play melody
    for (int i = 0; i < length; i++) {
        current_melody[i] = melody[i];
        current_durations[i] = durations[i];
    }
    current_length = length;  //store the length of the node which is playing now
    current_index = 0;  //store the index of the node which is playing now
    wholenote = (60000 * 4) / bpm;  //bpm is beats per minute, so 60000ms / bpm gives the duration of a quarter note, multiplying by 4 gives the duration of a whole note
    if (length > 0) {  //play the first node
        current_note_duration = wholenote / current_durations[0];
        playNote(current_melody[0], current_note_duration);
    }
}

void updateMusic(void) {
    if (is_playing_melody == 0) return;  //if not playing, do nothing
    
    if ((HAL_GetTick() - note_start_time) >= current_note_duration) {  //if the current node has finished playing, play the next node
        current_index++;
        
        if (current_index >= current_length) {  //if the melody has finished playing, stop the music
            is_playing_melody = 0;
            buzzer_off(&buzzer_cfg);
        } else {  //continue to play the next node
            int duration_ms = wholenote / current_durations[current_index];
            playNote(current_melody[current_index], duration_ms);
        }
    }
}

void stopMusic(void) {  //stop the music immediately
    is_playing_melody = 0;
    buzzer_off(&buzzer_cfg);
}
//you can add "int length = sizeof(melody) / sizeof(melody[0]);" in game.c to calculate the length more easier