#ifndef ENEMYSETTING_H
#define ENEMYSETTING_H
#include <stdint.h>

typedef enum{
    ENEMY_NORMAL,  //shooter
    ENEMY_HEAVY  //boomer
}ENEMY_TYPE;

typedef struct{
    ENEMY_TYPE type;  //type of enemy
    float x;  //location of enemy
    float y;
    float width;  //size of enemy
    float height;
    uint8_t is_active;  //enemy dead or alive
    int health;  //enemies hp
    float maxspeed;  //maximum enemy speed
    float current_speed;  //current speed of enemy
    
    int collision_damage;  //when the enemy have collision with player will casue collision damage
    int shoot_delay; //make sure ememy will not shoot  very fast
    uint8_t boomer;  //if the enemy is boomer, it will explode when it die, and cause explosion damage to player if player is in explosion radius
    int explosion_damage;  //the damage of explosion
    float explosion_radius;  //the radius of explosion


    float move_dx;  //move direction of enemy
    float move_dy;

    uint32_t move_start_time;  
    uint32_t move_duration;
    uint8_t is_moving;
}Enemy; 

void Enemy_Init(Enemy* enemy, ENEMY_TYPE type, float x, float y); // Initialize enemy with type and position

void Enemy_Deactivate(Enemy* e);  // Deactivate enemy (set is_active to 0)

uint8_t Enemy_IsInExplosionRadius(const Enemy* e, float player_x, float player_y);  // Check if player is in the enemy's explosion radius

void Enemy_UpdateMovement(Enemy* e);  // Update enemy movement based on its type and behavior
#endif  //ENEMYSETTING.H