/*
 * File:   main.c
 * Author: Bharath
 *
 * Created on 14 February, 2024, 5:40 PM
 */

// Include necessary header files for various components.
#include<xc.h>
#include "black_box.h"
#include "clcd.h"
#include "eeprom.h"
#include "main1.h"
#include "i2c.h"
#include "ds1307.h"
#include "UT1.h"

// Global variable declarations
State_t state;            // Variable to hold the current state of the system
unsigned int i, key, count, d = 1;  
unsigned char time[9], clock_reg[3]; 

// Function to initialize configurations
void init_config() {
    state = e_dashboard;  // Set initial state to e_dashboard (Dashboard view)
    init_i2c();           // Initialize I2C communication
    init_ds1307();        // Initialize DS1307 RTC (Real-Time Clock)
}

// Function to get the current time from DS1307 RTC
static void get_time(void) {
    // Read the time registers (hour, minute, second) from DS1307
    clock_reg[0] = read_ds1307(HOUR_ADDR);
    clock_reg[1] = read_ds1307(MIN_ADDR);
    clock_reg[2] = read_ds1307(SEC_ADDR);

    if (clock_reg[0] & 0x40) {
        time[0] = '0' + ((clock_reg[0] >> 4) & 0x01);  // Extract hour tens digit
        time[1] = '0' + (clock_reg[0] & 0x0F);         // Extract hour ones digit
    } else {
        time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);  // Extract hour tens digit (24-hour format)
        time[1] = '0' + (clock_reg[0] & 0x0F);         // Extract hour ones digit
    }

    time[2] = ':';
    time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F); // Extract minute tens digit
    time[4] = '0' + (clock_reg[1] & 0x0F);         // Extract minute ones digit
    time[5] = ':'; 
    time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F); // Extract second tens digit
    time[7] = '0' + (clock_reg[2] & 0x0F);         // Extract second ones digit
    time[8] = '\0'; 
}

void main(void) {
    init();        // Initialize all necessary components
    init_Mkp();    // Initialize keypad (assumed function)
    init_clcd();   // Initialize character LCD display
    init_config(); // Call the config initialization function
    
    while (1) {
        get_time(); // Update time on each loop iteration
        key = read_Mkp_Switch(STATE_CHANGE); // Read key press from the keypad (detecting state change)

        // If '#' key is pressed and d == 1, clear the screen and move to the main menu state
        if (key == '#' && d == 1) {
            CLEAR_DISP_SCREEN; // Clear display screen
            state = e_main_menu; // Set the state to main menu
            count++; // Increment the count 
        } 
        // If '0' key is pressed, reset and go to the dashboard state
        else if (key == '0') {
            CLEAR_DISP_SCREEN; // Clear display screen
            count = 0;          // Reset the count
            state = e_dashboard; // Set the state to dashboard view
        }

      
        switch (state) {
            case e_dashboard:
            
                CLEAR_DISP_SCREEN;  // Clear display screen
                view_dashboard();   // Call the function to display the dashboard
                break;

            case e_main_menu:
               
                display_main_menu(); // Display the main menu options
                break;

            case e_view_log:
           
                view_log();         // Call the function to display logs
                break;

            case e_download_log:
           
                download_log();     // Call the function to download logs
                break;

            case e_clear_log:
               
                clear_log();        // Call the function to clear logs
                break;

            case e_set_time:
             
                set_time();         // Call the function to set the time
                break;
        }
    }
}
