#include "Spaceship.h"
#include "LCD.h"
#include <math.h>

// Initialize spaceship
void Spaceship_Init(Spaceship_t* ship, float start_x, float start_y, float speed, uint8_t radius) {
    ship->x = start_x;// start point
    ship->y = start_y;
    // dx, dy represent the direction vector of the spaceship's movement.
    // can be (1, 0) for right, (0, 1) for down, (-1, 0) for left, (0, -1) for up.
    ship->dx = 1.0f;  // start moving right
    ship->dy = 0.0f;
    ship->speed = speed;
    ship->radius = radius;
}

// Update spaceship position and handle wall collisions
void Spaceship_Update(Spaceship_t* ship, uint16_t screen_width, uint16_t screen_height) {
    // Move spaceship
    // Moving straight in the direction of (dx, dy) with the given speed
    ship->x += ship->dx * ship->speed;
    ship->y += ship->dy * ship->speed;

    // Check wall collisions and bounce
    if (ship->x - ship->radius < 0 || ship->x + ship->radius > screen_width) {
        ship->dx = -ship->dx;
        ship->x = ship->x < ship->radius ? ship->radius : screen_width - ship->radius;
    }
    if (ship->y - ship->radius < 0 || ship->y + ship->radius > screen_height) {
        ship->dy = -ship->dy;
        ship->y = ship->y < ship->radius ? ship->radius : screen_height - ship->radius;
    }
}

// Draw spaceship on screen
void Spaceship_Draw(Spaceship_t* ship) {
    // Calculate triangle points based on position and direction
    // Nose (front point)
    int16_t nose_x = (int16_t)(ship->x + ship->dx * ship->radius);
    int16_t nose_y = (int16_t)(ship->y + ship->dy * ship->radius);
    
    // Left wing (smaller radius and perpendicular to direction)
    int16_t left_x = (int16_t)(ship->x - ship->dy * ship->radius * 0.7f);
    int16_t left_y = (int16_t)(ship->y + ship->dx * ship->radius * 0.7f);
    
    // Right wing (smaller radius and perpendicular to direction)
    int16_t right_x = (int16_t)(ship->x + ship->dy * ship->radius * 0.7f);
    int16_t right_y = (int16_t)(ship->y - ship->dx * ship->radius * 0.7f);
    
    // Draw the three sides of the triangle in red
    LCD_Draw_Line(nose_x, nose_y, left_x, left_y, LCD_COLOUR_2);   // Nose to left wing
    LCD_Draw_Line(left_x, left_y, right_x, right_y, LCD_COLOUR_2); // Left wing to right wing
    LCD_Draw_Line(right_x, right_y, nose_x, nose_y, LCD_COLOUR_2); // Right wing to nose
}

// Change direction (rotate by 45 degrees clockwise)
void Spaceship_ChangeDirection(Spaceship_t* ship) {
    const float cos45 = 0.707106781f;  // cos(45)
    const float sin45 = 0.707106781f;  // sin(45)
    
    // Rotate vector by 45° clockwise
    float new_dx = ship->dx * cos45 + ship->dy * sin45;
    float new_dy = -ship->dx * sin45 + ship->dy * cos45;
    
    // Normalize the vector to maintain speed
    float length = sqrtf(new_dx * new_dx + new_dy * new_dy);
    if (length > 0.0f) {
        ship->dx = new_dx / length;
        ship->dy = new_dy / length;
    }
}