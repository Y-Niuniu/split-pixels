#ifndef WEAPONSETTING_H
#define WEAPONSETTING_H
#include <stdint.h>

typedef enum {
    WEAPON_PISTOL,
    WEAPON_RIFLE,
    WEAPON_SHOTGUN,
    WEAPON_ROCKET
} WEAPON_TYPE;

typedef struct {
    int   bullet_count;
    float spread_angle;
    int   burst_interval_ms;
} WeaponFireMode;

extern const WeaponFireMode WEAPON_MODE_PISTOL;
extern const WeaponFireMode WEAPON_MODE_RIFLE;
extern const WeaponFireMode WEAPON_MODE_SHOTGUN;
extern const WeaponFireMode WEAPON_MODE_ROCKET;

const WeaponFireMode* Weapon_GetFireMode(WEAPON_TYPE type);

#endif // WEAPONSETTING_H
