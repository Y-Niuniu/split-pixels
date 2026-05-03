#include "weaponsetting.h"

const WeaponFireMode WEAPON_MODE_PISTOL  = {1, 0.0f,  0};
const WeaponFireMode WEAPON_MODE_RIFLE   = {3, 0.0f,  120};
const WeaponFireMode WEAPON_MODE_SHOTGUN = {7, 25.0f, 0};
const WeaponFireMode WEAPON_MODE_ROCKET  = {1, 0.0f,  0};

const WeaponFireMode* Weapon_GetFireMode(WEAPON_TYPE type) {
    switch (type) {
        case WEAPON_PISTOL:  return &WEAPON_MODE_PISTOL;
        case WEAPON_RIFLE:   return &WEAPON_MODE_RIFLE;
        case WEAPON_SHOTGUN: return &WEAPON_MODE_SHOTGUN;
        case WEAPON_ROCKET:  return &WEAPON_MODE_ROCKET;
    }
}