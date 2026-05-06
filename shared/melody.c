#include "melody.h"

#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123

#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247

#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494

#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988

#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976

#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_REST 0

int BM[] = {
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
        NOTE_E4, NOTE_G4, NOTE_D4,

        NOTE_REST
};
int BD[] = {
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
        4, 8, 4,

        0
};


int SWM[] = {
    NOTE_D4,NOTE_D4, NOTE_D4,

    NOTE_G4,  NOTE_D5,

    NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G5, NOTE_D5,

    NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G5, NOTE_D5,

    NOTE_C5, NOTE_B4, NOTE_C5, NOTE_A4,

    NOTE_D4, NOTE_D4, NOTE_D4,

    NOTE_G4, NOTE_D5,

    NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G5, NOTE_D5,

    NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G5, NOTE_D5,

    NOTE_C5, NOTE_B4, NOTE_C5, NOTE_A4, NOTE_D4, NOTE_D4,

    NOTE_E4, NOTE_E4,

    NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G4,

    NOTE_G4, NOTE_B4, NOTE_A4, NOTE_E4,

    NOTE_FS4, NOTE_D4, NOTE_D4,

    NOTE_E4, NOTE_E4,

    NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G4,

    NOTE_D5, NOTE_A4,

    NOTE_D4, NOTE_D4,

    NOTE_E4, NOTE_E4,
    
    NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_REST,

    NOTE_G4, NOTE_B4, NOTE_A4, NOTE_E4,
    
    NOTE_FS4, NOTE_D4, NOTE_D4,

    NOTE_CS5, NOTE_C5,

    NOTE_B4, NOTE_AS4,

    NOTE_A4, NOTE_G4,

    NOTE_DS5,

    

    
    
    
    
    NOTE_REST
};

int SWD[] = {
    12,12,12,

    2,2,

    12,12,12,2,4,

    12,12,12,2,4,

    12,12,12,2,

    12,12,12,

    2,2,

    12,12,12,2,4,

    12,12,12,2,4,

    12,12,12,2,8,8,

    4,4,

    12,12,12,12,

    12,12,12,12,

    4,8,8,

    4,4,

    12,12,12,12,

    4,2,

    8,8,

    4,4,

    12,12,12,12,8,

    12,12,12,12,

    4,8,8,

    12,12,

    12,12,

    12,12,

    2,







    0
};


int ACM[] = {
        NOTE_F4, NOTE_D4, NOTE_F4, NOTE_D4, NOTE_F4, NOTE_D4,

        NOTE_E4, NOTE_D4, NOTE_E4, NOTE_D4,

        NOTE_F4, NOTE_D4, NOTE_F4, NOTE_D4, NOTE_F4, NOTE_D4,

        NOTE_G4, NOTE_D4, NOTE_G4, NOTE_D4,

        NOTE_F4, NOTE_D4, NOTE_F4, NOTE_D4,NOTE_F4, NOTE_D4,
        
        NOTE_E4, NOTE_D4, NOTE_E4, NOTE_D4,
        
        NOTE_F4, NOTE_D4,NOTE_F4, NOTE_D4, NOTE_F4, NOTE_D4,
        
        NOTE_E4, NOTE_D4, NOTE_A4, NOTE_D5,
        
        NOTE_F5, NOTE_D5, NOTE_F5, NOTE_D5,NOTE_E5, NOTE_D5,
        
        NOTE_F5, NOTE_D5, NOTE_F5, NOTE_D5,
        
        NOTE_F5, NOTE_D5,

        NOTE_A5,

        NOTE_REST,
        
        NOTE_F4, NOTE_E4, NOTE_D4,
        
        NOTE_A3,
        
        NOTE_C4,
        
        NOTE_D4, 
        
        NOTE_F4, NOTE_E4, NOTE_D4,
        
        NOTE_F4,
        
        NOTE_C4,
        
        NOTE_D4,
        
        NOTE_A4,
        
        NOTE_REST,
        
        NOTE_F4,

        NOTE_G4,

        NOTE_REST,
        
        NOTE_G4,
        
        NOTE_A4,
        
        NOTE_AS4,
        
        NOTE_AS4, NOTE_A4, NOTE_G4,
        
        NOTE_A4,

        NOTE_E4,
        
        NOTE_D4,
        
        NOTE_E4,
        
        NOTE_F4, NOTE_E4, NOTE_D4,
        
        NOTE_A3,
        
        NOTE_C4,

        NOTE_D4,
        


        //NOTE_F5, NOTE_E5, NOTE_D5,
        
        //NOTE_A4,
        
        //NOTE_C5,
        
        //NOTE_D5,
        
        //NOTE_F5, NOTE_E5, NOTE_D5,
        
        //NOTE_F5,
        
        //NOTE_C5,
        
        //NOTE_D5,
        
        //NOTE_A5,

        //NOTE_F5,

        //NOTE_FS5,
        
        //NOTE_G5,
        
        //NOTE_A5,
        
        //NOTE_AS5,
        
        //NOTE_A5,
        
        //NOTE_G5,
        
        //NOTE_A5,
        
        //NOTE_REST,

        //NOTE_E5,
        
        //NOTE_AS5,

        //NOTE_REST,
        
        //NOTE_AS5,
        
        //NOTE_D5,
        
        //NOTE_E5,
        
        //NOTE_F5, NOTE_E5, NOTE_D5,
        
        //NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5,

        //NOTE_REST
};

int ACD[] = {
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8,8, 8, 8, 8, 8, 8, 8, 8,
        2,

        4,
        
        6, 6, 6, 2, 2, 2,
        6, 6, 6, 2, 4, 4, 4, 16,
        8, 4, 32,
        4, 4, 2,
        6, 6, 6, 2,
        4, 4, 4,
        6, 6, 6, 2, 2, 1,

        //6, 6, 6, 2, 2, 2, 4,
        //4, 4, 2, 2, 4, 4, 8,
        //2, 4, 4, 2, 4, 4, 4, 8,
        //8, 2, 16, 4, 4, 4, 6, 6, 6,
        //2, 2, 1, 4,

        0
};


int PVZWINM[] = {
    NOTE_B4, NOTE_A4, NOTE_F4,
    NOTE_C5, NOTE_B4, NOTE_A4,
    NOTE_F4, NOTE_E4, NOTE_C4,
    NOTE_B3, NOTE_A3, NOTE_F3,
    NOTE_A3, NOTE_B3, NOTE_B3,
    NOTE_C4, NOTE_E4, NOTE_E4,
    NOTE_FS4,

    NOTE_REST
};

int PVZWIND[] = {
        8,8,4,
        6,8,6,
        6,8,6,
        6,8,6,
        8,6,8,
        8,6,8,
        2,

        0
};

int PVZLOSEM[]= {
        NOTE_E5, NOTE_F4, NOTE_C5, NOTE_E4,

        NOTE_DS4, NOTE_E4,
        NOTE_DS4, NOTE_E4,
        NOTE_DS4, NOTE_E4,
        NOTE_DS4, NOTE_E4,
        NOTE_DS4, NOTE_E4,
        NOTE_DS4, NOTE_E4,
        NOTE_DS4, NOTE_E4,
        NOTE_DS4, NOTE_E4,
        NOTE_DS4,



        NOTE_REST
};
int PVZLOSED[]= {
        4,4,4,4,
        32,32,
        32,32,
        32,32,
        32,32,
        32,32, 
        32,32,
        32,32,
        32,32,
        2,


        0
};