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
    uint8_t is_active;  //enemy dead or alive

    char name[32];  //every new enemies have their own name, kindly like[1 2] and 1 and 2 is diferent words

    int health;  //enemies hp
    float speed;  //enemy speed
    int collision_damage;  //when the enemy have collision with player will casue collision damage

    int shoot_delay;

    uint8_t boomer;
    uint8_t will_explode_on_death;
    int explosion_damage;
    float explosion_radius;
}Enemy; 

// Initialize enemy
void Enemy_Init(Enemy* enemy, ENEMY_TYPE type, float x, float y);
void Enemy_RandomName(char* output, int max_len);

#endif  //ENEMYSETTING.H