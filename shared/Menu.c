#include "Menu.h"
#include "LCD.h"
#include "InputHandler.h"
#include "Joystick.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>
#include"game_1.h"
#include <stdbool.h>
extern ST7789V2_cfg_t cfg0;  // LCD configuration from main.c
extern Joystick_cfg_t joystick_cfg;  // Joystick configuration
extern Joystick_t joystick_data;     // Current joystick readings
extern Joystick_cfg_t joystick_cfg2;  // Second joystick configuration
extern Joystick_t joystick_data2;     // Second joystick readings
extern InputState current_input;      // Current input state

static const char* background_story[]={

    "In a galaxy far far away",
    "the most tellent people",
    "will attend a great game",
    "called split pixels.",
    "",
    "Prove yourself have ",
    "strength and courage ",
    "to be free,",
    "The game challenge the skills",
    "of combat, colloberation",
    "and strategy.",
    "",
    "The winner team will have",
    "great rewards.",
    "",
    "JOIN THE SPLIT PIXELS!",
    "Begin your adventure", 
    "Become a legend!",
    "Become a Split",  
    "Pixel master"
};

#define NUM_BACKGROUND_LINES (sizeof(background_story) / sizeof(char*))
#define LINE_HEIGHT 20
#define TITLE_HEIGHT 40
#define TEXT_HEIGHT (LINE_HEIGHT * NUM_BACKGROUND_LINES + TITLE_HEIGHT)


static int p1_selection=0;
static int p2_selection=0;
static uint32_t title_start_time = 0;  // For title display timer
static bool title_finished = false;    // Whether title phase is finished

// Menu options
static const char* menu_options[] = {
    "Game 1",
    "Game 2", 
    "Game 3"
};
#define NUM_MENU_OPTIONS 3

// Frame rate for menu (in milliseconds)
#define MENU_FRAME_TIME_MS 30  // ~33 FPS

/**
 * @brief Render the home menu screen
 */
static void render_title(void) {
    LCD_Fill_Buffer(0);
    
    // Title
    LCD_printString("SPLIT PIXELS", 20, 120, 10, 3);
    
    LCD_Refresh(&cfg0);
}

static void render_story(int16_t line_offset) {
    LCD_Fill_Buffer(0);
    
    // Background story text
    for (int i = 0; i < NUM_BACKGROUND_LINES; i++) {
        int16_t line_y =line_offset+TITLE_HEIGHT+(i-1)*LINE_HEIGHT;
        if (line_y >= 0 && line_y < 240) {  // Only render lines that are visible on screen
            LCD_printString((char*)background_story[i], 0, line_y, 14, 2);
        }
    }
    
    LCD_Refresh(&cfg0);
}



static void render_home_menu(MenuSystem* menu) {
    LCD_Fill_Buffer(0);
    

    // Title
    LCD_printString("SPLIT PIXELS", 20, 10, 10, 3);
    
    // Menu options with selection highlight
    for (int i = 0; i < NUM_MENU_OPTIONS; i++) {
        uint16_t y_pos = 70 + (i * 40);
        // Print menu option text
        LCD_printString((char*)menu_options[i], 60, y_pos, 1, 2);
        
        if (i == p1_selection && i == p2_selection) {
            LCD_Draw_Circle(40, y_pos, 5, 3, 1);
            LCD_Draw_Rect(140, y_pos, 10, 10, 5, 1);
        } else {
            if (i == p1_selection) {
                LCD_Draw_Circle(40, y_pos, 5, 3, 0);
            }
            if (i == p2_selection) {
                LCD_Draw_Rect(140, y_pos, 10, 10, 5, 0);
            }
        }
    }
    
    // Instructions
    LCD_printString("Both player press Joystick button to start", 50, 240, 1, 1);
    
    LCD_Refresh(&cfg0);
}

// ==============================================
// PUBLIC API IMPLEMENTATION
// ==============================================

void Menu_Init(MenuSystem* menu) {
    menu->selected_option = 0;
}

MenuState Menu_Run(MenuSystem* menu) {
    static Direction p1_last_direction = CENTRE;
    static Direction p2_last_direction = CENTRE;  // Track last direction for debouncing
    static int16_t line_offset = 0;  // For scrolling background story
    static bool story_finished = false;
    MenuState selected_game = MENU_STATE_HOME;  // Which game was selected
    
    // Initialize for this run
    title_finished = false;
    story_finished = false;
    line_offset = 0;
    p1_selection = 0;
    p2_selection = 0;
    title_start_time = HAL_GetTick();
    
    // Menu's own loop - runs until game is selected
    while (1) {
        uint32_t frame_start = HAL_GetTick();
        
        // Read input
        Input_Read();
        
        // Read current joystick position
        Joystick_Read(&joystick_cfg, &joystick_data);
        Joystick_Read(&joystick_cfg2, &joystick_data2);
        
        if (!title_finished) {
            if (HAL_GetTick() - title_start_time >= 3000) {
                title_finished = true;
            }
            
            if (current_input.btn3_pressed || current_input.btn9_pressed) {
                title_finished = true;
            }
           
            render_title();
        } else if (!story_finished) {
            // Story phase: scroll story and allow skip
            if (current_input.btn3_pressed || current_input.btn9_pressed) {
                story_finished = true;
            }
            
            // Render story
            render_story(line_offset);

            // Update line offset for scrolling
            line_offset -= 1;  // Scroll up slowly
            if (line_offset < -TEXT_HEIGHT) {
                story_finished = true;  // Story finished scrolling
            }
        } else {
            // Menu phase: navigation and selection
            // Handle joystick navigation (up/down to select option)
            Direction p1_current_direction = joystick_data.direction;
            
            if (p1_current_direction == S && p1_last_direction != S) {  // Joystick pushed DOWN
                // Move selection down
                p1_selection++;
                if (p1_selection >= NUM_MENU_OPTIONS) {
                    p1_selection = 0;  // Wrap around
                }
            } 
            else if (p1_current_direction == N && p1_last_direction != N) {  // Joystick pushed UP
                // Move selection up
                if (p1_selection == 0) {
                    p1_selection = NUM_MENU_OPTIONS - 1;  // Wrap around
                } else {
                    p1_selection--;
                }
            }
            
            p1_last_direction = p1_current_direction;
            Direction p2_current_direction = joystick_data2.direction;
            
            if (p2_current_direction == S && p2_last_direction != S) {  // Joystick pushed DOWN
                // Move selection down
                p2_selection++;
                if (p2_selection >= NUM_MENU_OPTIONS) {
                    p2_selection = 0;  // Wrap around
                }
            } 
            else if (p2_current_direction == N && p2_last_direction != N) {  // Joystick pushed UP
                // Move selection up
                if (p2_selection == 0) {
                    p2_selection = NUM_MENU_OPTIONS - 1;  // Wrap around
                } else {
                    p2_selection--;
                }
            }
            
            p2_last_direction = p2_current_direction;
            
            // Handle button press to select current option
            if ((p1_selection == p2_selection) && (current_input.btn3_pressed) && (current_input.btn9_pressed)) {
                // User pressed button - select the highlighted option
                if (p1_selection == 0) {
                    selected_game = MENU_STATE_GAME_1;
                } else if (p1_selection == 1) {
                    selected_game = MENU_STATE_GAME_2;
                } else if (p1_selection == 2) {
                    selected_game = MENU_STATE_GAME_3;
                }
                break;  // Exit menu loop - game selected!
            }
            
            // Render menu
            render_home_menu(menu);
        }
        // Frame timing - wait for remainder of frame time
        uint32_t frame_time = HAL_GetTick() - frame_start;
        if (frame_time < MENU_FRAME_TIME_MS) {
            HAL_Delay(MENU_FRAME_TIME_MS - frame_time);
        }
    }
    
    return selected_game;  // Return which game was selected
}