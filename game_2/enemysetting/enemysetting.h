#ifndef ENEMYSETTING_H
#define ENEMYSETTING_H
#include <stdint.h>
#include "playersetting/playersetting.h"

typedef enum{
    ENEMY_NORMAL,  //shooter
    ENEMY_HEAVY  //boomer
}ENEMY_TYPE;

typedef struct{
    ENEMY_TYPE type;  //type of enemy
    float x;  //location of enemy
    float y;
    float radius;  //for collision detection
    uint8_t is_active;  //enemy dead or alive
    int health;  //enemies hp
    float maxspeed;  //maximum enemy speed
    float current_speed;  //current speed of enemy
    
    int collision_damage;  //when the enemy have collision with player will casue collision damage
    uint8_t boomer;  //if the enemy is boomer, it will explode when it die, and cause explosion damage to player if player is in explosion radius
    int explosion_damage;  //the damage of explosion
    float explosion_radius;  //the radius of explosion

    float move_dx;  //move direction of enemy
    float move_dy;

    uint32_t move_start_time;   //moving logic 
    uint32_t move_duration;
    uint8_t is_moving;

    uint32_t last_shot_time;  //shooting logic
    uint32_t next_shot_delay;
}Enemy; 

void Enemy_Init(Enemy* enemy, ENEMY_TYPE type, float x, float y); // Initialize enemy with type and position

void Enemy_Deactivate(Enemy* e);  // Deactivate enemy (set is_active to 0)

uint8_t Enemy_IsInExplosionRadius(const Enemy* e, float x, float y);  // Check if player is in the enemy's explosion radius

void Enemy_CheckExplosionDamage(const Enemy* e, Player* p1, Player* p2);  // Check if players are in explosion radius and apply damage if necessary

void Enemy_UpdateMovement(Enemy* e, float p1_x, float p1_y, uint8_t p1_alive, float p2_x, float p2_y, uint8_t p2_alive);  // Update enemy movement based on its type and behavior

uint8_t Enemy_TryShoot(Enemy* e);  // Check if enemy can shoot based on shoot delay, return 1 if it can shoot, otherwise return 0

void Enemy_Draw(Enemy* e);  // Draw enemy on screen with different colors based on type
#endif  //ENEMYSETTING.H