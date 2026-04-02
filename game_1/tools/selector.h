#ifndef SELECTOR_H
#define SELECTOR_H
#include <stdint.h>
typedef enum{
    RESUME = 0,
    EXIT,
}operation_t;
typedef struct{
    uint8_t operation;
}select_system_t;
void Selector_Init(select_system_t* selector);
operation_t Selector_Run(select_system_t* selector);
#endif 
