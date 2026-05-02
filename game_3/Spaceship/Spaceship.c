#include "Spaceship.h"
#include "LCD.h"
#include <math.h>

// Simple triangle fill function using scanline algorithm
void LCD_Fill_Triangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint8_t colour) {
    // Sort points by y-coordinate (y1 <= y2 <= y3)
    if (y1 > y2) {
        int16_t temp;
        temp = x1; x1 = x2; x2 = temp;
        temp = y1; y1 = y2; y2 = temp;
    }
    if (y2 > y3) {
        int16_t temp;
        temp = x2; x2 = x3; x3 = temp;
        temp = y2; y2 = y3; y3 = temp;
    }
    if (y1 > y2) {
        int16_t temp;
        temp = x1; x1 = x2; x2 = temp;
        temp = y1; y1 = y2; y2 = temp;
    }
    
    // If it's a flat triangle, return
    if (y1 == y3) return;
    
    // Fill the triangle using scanlines
    for (int16_t y = y1; y <= y3; y++) {
        float x_left = 9999.0f;   // Large number
        float x_right = -9999.0f; // Small number
        
        // Check each edge for intersection with this scanline
        // Edge 1-2
        if (y >= y1 && y <= y2 && y2 > y1) {
            float t = (float)(y - y1) / (float)(y2 - y1);
            float x = x1 + t * (x2 - x1);
            if (x < x_left) x_left = x;
            if (x > x_right) x_right = x;
        }
        
        // Edge 1-3
        if (y >= y1 && y <= y3 && y3 > y1) {
            float t = (float)(y - y1) / (float)(y3 - y1);
            float x = x1 + t * (x3 - x1);
            if (x < x_left) x_left = x;
            if (x > x_right) x_right = x;
        }
        
        // Edge 2-3
        if (y >= y2 && y <= y3 && y3 > y2) {
            float t = (float)(y - y2) / (float)(y3 - y2);
            float x = x2 + t * (x3 - x2);
            if (x < x_left) x_left = x;
            if (x > x_right) x_right = x;
        }
        
        // Draw horizontal line if we found valid intersections
        if (x_left < x_right) {
            LCD_Draw_Line((int16_t)x_left, y, (int16_t)x_right, y, colour);
        }
    }
}

// Initialize spaceship
void Spaceship_Init(Spaceship_t* ship, float start_x, float start_y, float speed, uint8_t radius, uint8_t colour) {
    ship->x = start_x;// start point
    ship->y = start_y;
    // dx, dy represent the direction vector of the spaceship's movement.
    // can be (1, 0) for right, (0, 1) for down, (-1, 0) for left, (0, -1) for up.
    ship->dx = 1.0f;  // start moving right
    ship->dy = 0.0f;
    ship->speed = speed;
    ship->radius = radius;
    ship->colour = colour;
    ship->active = 1;  // spaceship is active
}

// Update spaceship position and handle wall collisions
void Spaceship_Update(Spaceship_t* ship, uint16_t screen_width, uint16_t screen_height) {
    if (!ship->active) return;  // do nothing if not active

    // Move spaceship
    // Moving straight in the direction of (dx, dy) with the given speed
    ship->x += ship->dx * ship->speed;
    ship->y += ship->dy * ship->speed;

    // Check wall collisions and clamp position
    if (ship->x - ship->radius < 0) {
        ship->x = ship->radius;
    } else if (ship->x + ship->radius > screen_width) {
        ship->x = screen_width - ship->radius;
    }
    if (ship->y - ship->radius < 0) {
        ship->y = ship->radius;
    } else if (ship->y + ship->radius > screen_height) {
        ship->y = screen_height - ship->radius;
    }
}

// Draw spaceship on screen
void Spaceship_Draw(Spaceship_t* ship) {
   
    int16_t nose_x = (int16_t)(ship->x + ship->dx * ship->radius);
    int16_t nose_y = (int16_t)(ship->y + ship->dy * ship->radius);
    
    // Left wing (smaller radius and perpendicular to direction)
    int16_t left_x = (int16_t)(ship->x - ship->dy * ship->radius * 0.7f);
    int16_t left_y = (int16_t)(ship->y + ship->dx * ship->radius * 0.7f);
    
    // Right wing (smaller radius and perpendicular to direction)
    int16_t right_x = (int16_t)(ship->x + ship->dy * ship->radius * 0.7f);
    int16_t right_y = (int16_t)(ship->y - ship->dx * ship->radius * 0.7f);
    
    // Fill the triangle with ship colour
    LCD_Fill_Triangle(nose_x, nose_y, left_x, left_y, right_x, right_y, ship->colour);
    
    // Draw the outline of the spaceship
    LCD_Draw_Line(nose_x, nose_y, left_x, left_y, ship->colour);   // Nose to left wing
    LCD_Draw_Line(left_x, left_y, right_x, right_y, ship->colour); // Left wing to right wing
    LCD_Draw_Line(right_x, right_y, nose_x, nose_y, ship->colour); // Right wing to nose
}

// Change direction (rotate by 12 degrees clockwise)
void Spaceship_ChangeDirection(Spaceship_t* ship) {
    const float cos12 = 0.9781476007f;  // cos(12°)
    const float sin12 = 0.2079116908f;  // sin(12°)
    
    // Rotate vector by 12° clockwise
    float new_dx = ship->dx * cos12 + ship->dy * sin12;
    float new_dy = -ship->dx * sin12 + ship->dy * cos12;
    
    // Normalize the vector to maintain speed
    float length = sqrtf(new_dx * new_dx + new_dy * new_dy);
    if (length > 0.0f) {
        ship->dx = new_dx / length;
        ship->dy = new_dy / length;
    }
}

// Deactivate spaceship
void Spaceship_Deactivate(Spaceship_t* ship) {
    ship->active = 0;
}