#include "interact.h"
#include "engine.h"
#include "data/plantdata.h"
#include "data/zombiedata.h"
#include "LCD.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sprites/zombies.h"

#define MAX_ZOMBIES 24//we have 3*5=15 zombies this for Reserved for the upper limit 
static int zombie_count = 0;

static uint32_t game_start_time = 0;
static uint32_t last_generate_time = 0;
static uint32_t next_generate_time = 0;
static int game_score = 0;
static bool game_victory = false;

static int get_y(int row){
    extern const uint16_t b_y[];
    return b_y[row];
}

void Zombie_Init(void){
    zombie_count=0;
    for(int i=0;i<MAX_ZOMBIES;i++){
        zombie_instances[i].is_alive = false;
    }
    game_start_time = HAL_GetTick();
    last_generate_time = HAL_start_time;
    next_generate_time = 15000+rand()%5000;
    game_score = 0;
    game_victory = false;
}

void zombie_generate(zombietype_t type, int row){
if(zombie_count>=MAX_ZOMBIES){
    return;
}
for(int i=0;i<MAX_ZOMBIES;i++){
    if(!zombie_instances[i] .is_alive){
        zombie_instances[i].type = type;
        zombie_instances[i].row = row;
        zombie_instances[i].x = 240.0f;
        zombie_instances[i].y = get_y(row);
        zombie_instances[i].health = zombie_status[type].max_health;
        zombie_instances[i].is_alive = true;
        zombie_instances[i].attack_interval = 0;
        zombie_count++;
        break;
    }
}
}
void Zombie_Update(uint32_t delta_ms){
// "alive" zombies moves forward 
for(int i=0;i<MAX_ZOMBIES;i++){
    if(!zombie_instances[i].is_alive)
    continue;
    float speed = zombie_status[zombie_instances[i].type].speed_pix_per_sec;
    float move=speed*delta_ms/1000.0f;
    zombie_instances[i].x-=move;
    if(zombie_instances[i].x<0)zombie_instances[i].x=0;
}
// if more than 3min you defence secceed then win
uint32_t now = HAL_GetTick();
uint32_t passed_time=(now-game_start_time)/1000;
if(passed_time>=180&&!game_victory){
    game_victory=true;
    return;
}
if(game_victory)return;

if(now-last_generate_time>=next_generate_time){
last_generate_time=now;
next_generate_time=15000+(rand()%5000);
int zombies_per_row=1;
int bucket_chance=0;
if(passed_time<60){
zombies_per_row=1+(rand()%2);
bucket_chance=0;
}
else if(passed_time<120){
zombies_per_row=2+(rand()%2);
bucket_chance=40;
}
else{
zombies_per_row=3+(rand()%2);
bucket_chance=60;
}
}

for(int row=0;row<3;row++){
    for(int i=0;i<zombies_per_row;i++){
        zombietype_t type=normal;
        if(bucket_chance>0&&rand()%100<bucket_chance){
            type=bucket;
        }
        zombie_generate(type, row);
    }
}

}

void Zombie_Render(void) {
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        if (zombie_instances[i].is_alive) {
            const uint8_t* sprite = zombie_status[zombie_instances[i].type].sprite;
            if (sprite) {
                LCD_Draw_Sprite(zombie_instances[i].x, zombie_instances[i].y, 40, 27, sprite);
            }
        }
    }
}