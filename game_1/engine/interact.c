#include "interact.h"
#include "engine.h"
#include "data/plantdata.h"
#include "data/zombiedata.h"
#include "LCD.h"
#include "stm32l4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sprites/zombies.h"

zombieinstance_t zombie_instances[MAX_ZOMBIES];
static int zombie_count = 0;

static uint32_t game_start_time = 0;
static uint32_t last_generate_time = 0;
static uint32_t next_generate_time = 0;
uint32_t last_damage_time = 0;
static int game_score = 0;
static bool game_victory = false;
static bool game_over = false;

// FMS for Wave generation
typedef enum {
    WAVE_IDLE,
    WAVE_SPAWNING_ROW_0,
    WAVE_SPAWNING_ROW_1,
    WAVE_SPAWNING_ROW_2
} WaveState_t;

static WaveState_t wave_state = WAVE_IDLE;
static uint32_t wave_spawn_delay_timer = 0;
static int wave_zombies_per_row = 0;
static int wave_bucket_chance = 0;

extern const uint16_t b_y[];

static int get_y(int row){
    return b_y[row];
}

void Zombie_Init(void){
    zombie_count=0;
    for(int i=0;i<MAX_ZOMBIES;i++){
        zombie_instances[i].is_alive = false;
        zombie_instances[i].hit_time = 0;
        zombie_instances[i].is_attacking = false;
        zombie_instances[i].hit_time = 0;
        zombie_instances[i].attack_interval = 0;
    }
    game_start_time = HAL_GetTick();
    last_generate_time = game_start_time;
    next_generate_time = 15000+rand()%5000;
    game_score = 0;
    game_victory = false;
    game_over = false;
    wave_state = WAVE_IDLE;
}
void Zombie_ResetGameStartTime(void){
    game_start_time = HAL_GetTick();
    last_generate_time = game_start_time;
}
void zombie_generate(zombietype_t type, int row){
if(zombie_count>=MAX_ZOMBIES){
    return;
}
for(int i=0;i<MAX_ZOMBIES;i++){
    if(!zombie_instances[i] .is_alive){
        zombie_instances[i].type = type;
        zombie_instances[i].row = row;
        zombie_instances[i].x = 240;
        zombie_instances[i].y = get_y(row);
        zombie_instances[i].health = zombie_status[type].max_health;
        zombie_instances[i].is_alive = true;
        zombie_instances[i].is_attacking = false;
        zombie_instances[i].attack_interval = 0;
        zombie_count++;
        break;
    }
}
}
void Zombie_Update(uint32_t delta_ms){
// "alive" zombies moves forward if not attacking
for(int i=0;i<MAX_ZOMBIES;i++){
    if(!zombie_instances[i].is_alive)
    continue;
    if (!zombie_instances[i].is_attacking) {
        float speed = zombie_status[zombie_instances[i].type].speed_pix_per_sec;
        float move=speed*delta_ms/1000.0f;
        zombie_instances[i].x-=move;
        if(zombie_instances[i].x < 0){
            zombie_instances[i].x = 0;
            game_over = true;
        }
    }
}

uint32_t now = HAL_GetTick();
uint32_t passed_time=(now-game_start_time)/1000;
if(passed_time>=180&&!game_victory){
    game_victory=true;
    return;
}
if(game_victory)return;



//  Start a new wave if the state machine is idle
if (wave_state == WAVE_IDLE && now - last_generate_time >= next_generate_time) {
    last_generate_time = now;
    next_generate_time = 20000 + (rand() % 5000); // Time until NEXT wave starts
    if (passed_time < 60) {
        wave_zombies_per_row = 1 + (rand() % 2);
        wave_bucket_chance = 0;
    } else if (passed_time < 120) {
        wave_zombies_per_row = 2 + (rand() % 2);
        wave_bucket_chance = 40;
    } else {
        wave_zombies_per_row = 3 + (rand() % 2);
        wave_bucket_chance = 60;
    }

  
    wave_state = WAVE_SPAWNING_ROW_0;
    wave_spawn_delay_timer = now; // Spawn first row
}

// Process the currently active wave state
if (wave_state != WAVE_IDLE && now >= wave_spawn_delay_timer) {
    int row_to_spawn = -1;
    WaveState_t next_state = WAVE_IDLE;

    switch(wave_state) {
        case WAVE_SPAWNING_ROW_0:
            row_to_spawn = 0;
            next_state = WAVE_SPAWNING_ROW_1;
            break;
        case WAVE_SPAWNING_ROW_1:
            row_to_spawn = 1;
            next_state = WAVE_SPAWNING_ROW_2;
            break;
        case WAVE_SPAWNING_ROW_2:
            row_to_spawn = 2;
            next_state = WAVE_IDLE; 
            break;
        case WAVE_IDLE:
        default:
            break;
    }

    // Action: Spawn zombies if a valid row was determined
    if (row_to_spawn != -1) {
        for (int i = 0; i < wave_zombies_per_row; i++) {
            zombietype_t type = normal;
            if (wave_bucket_chance > 0 && rand() % 100 < wave_bucket_chance) {
                type = bucket;
            }
            zombie_generate(type, row_to_spawn);
        }
    }

   
    wave_state = next_state;

    // If the wave is not finished, set the timer for the next row
    if (wave_state != WAVE_IDLE) {
        wave_spawn_delay_timer = now + 3000 + (rand() % 2001);
    }
}
    }






void Zombie_Render(void) {
    // Animation tick: alternate (0/1) every 300ms — same beat for all zombies
   uint32_t now = HAL_GetTick();
    uint8_t anim_phase = (HAL_GetTick() / 300) % 2;
    
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        if (!zombie_instances[i].is_alive) continue;
         bool just_hit = (zombie_instances[i].hit_time != 0) &&((now - zombie_instances[i].hit_time) < HIT_FLASH);
        const uint8_t** frames = NULL;
        int frame_idx = 0;
        bool attacking = zombie_instances[i].is_attacking;

        switch (zombie_instances[i].type) {
            case normal:
                // normal: walk = 0<->1, attack = 0<->2
                frames = TEST_ZOMBIE1_FRAMES;
              if (attacking) {
            if (anim_phase == 0) {
                if (just_hit) {
                    frame_idx = 1;
                } else {
                    frame_idx = 0;
                }
            } else {
                if (just_hit) {
                    frame_idx = 5;
                } else {
                    frame_idx = 4;
                }
            }
        } else {
            if (anim_phase == 0) {
             if (just_hit) {
            frame_idx = 1;
                } else {
            frame_idx = 0;
            }
            } else {
                if (just_hit) {
            frame_idx = 3;
             } else {
            frame_idx = 2;
            }
        }
    }
                break;
           case bucket:
                frames = TEST_ZOMBIE2_FRAMES;
                 if (attacking) {
            if (anim_phase == 0) {
                if (just_hit) {
                    frame_idx = 1;
                } else {
                    frame_idx = 0;
                }
            } else {
                if (just_hit) {
                    frame_idx = 5;
                } else {
                    frame_idx = 4;
                }
            }
        } else {
            if (anim_phase == 0) {
             if (just_hit) {
            frame_idx = 1;
                } else {
            frame_idx = 0;
            }
            } else {
                if (just_hit) {
            frame_idx = 3;
             } else {
            frame_idx = 2;
            }
        }
    }
                break;
                default:
                break;
        }
        if (!frames) continue;
        zombietype_t type=zombie_instances[i].type;
        uint8_t h = zombie_status[type].h;
        uint8_t w = zombie_status[type].w;
        const uint8_t* sprite = frames[frame_idx];
        if (sprite) {
            LCD_Draw_Sprite((int16_t)zombie_instances[i].x, zombie_instances[i].y, h, w, sprite);
        }
    }
 }


extern Planting_t battle_grid[][5];
extern uint16_t plant_health[][5];
extern const uint16_t b_x[];

 bool Damage_Determination(void){
      bool hit = false;

      // shooter damage logic
      for(int row=0;row<3;row++){
          for(int col=0;col<5;col++){
              if(battle_grid[row][col]==plant_shooter){
                  zombieinstance_t* target_zombie = NULL;
                  int max_x = -1;
                  for(int i=0;i<MAX_ZOMBIES;i++){
                      if(zombie_instances[i].is_alive && zombie_instances[i].row == row){
                          if(zombie_instances[i].x > max_x){
                              max_x = zombie_instances[i].x;
                              target_zombie = &zombie_instances[i];
                          }
                      }
                  }
                  if(target_zombie != NULL){
                      target_zombie->health -= plant_status[plant_shooter].DPS;
                      target_zombie->hit_time = HAL_GetTick(); 
                      hit = true;
                      if(target_zombie->health <= 0){
                          target_zombie->is_alive = false;
                          zombie_count--;
                          Score_Add(zombie_status[target_zombie->type].score);
                      }
                  }
              }
          }
      }

      // zombie attack logic
      for(int i=0;i<MAX_ZOMBIES;i++){
          if(!zombie_instances[i].is_alive) continue;

          int row = zombie_instances[i].row;
          int col = -1;
          bool plant_in_front = false;

          for(int c=0; c<5; c++){
              if (zombie_instances[i].x >= b_x[c] && zombie_instances[i].x < b_x[c] + cell_width) {
                  if (battle_grid[row][c] != plant_none) {
                      col = c;
                      plant_in_front = true;
                      break;
                  }
              }
          }

          if(plant_in_front){
              zombie_instances[i].is_attacking = true;
              if(battle_grid[row][col] != plant_bomb){
                  int damage = zombie_status[zombie_instances[i].type].DPS;
                  plant_health[row][col] -= damage;
                  if(plant_health[row][col] <= 0){
                      battle_grid[row][col] = plant_none;
                      plant_health[row][col] = 0;
                      zombie_instances[i].is_attacking = false;
                  }
              }
          } else {
              zombie_instances[i].is_attacking = false;
          }
      }

      return hit;
  }
void Score_Add(int score){
    game_score+=score;
}

int Score_Get(void){
    return game_score;
}

uint32_t Game_time_pass(void){

    return (HAL_GetTick() - game_start_time) / 1000;
}
bool  IsVictory(){
    return game_victory;
}

bool IsGameOver(void){
    return game_over;
}