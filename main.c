/*
 * File:   main.c
 * Author: Bharath
 * PROJECT:CAR BLACK BOX
 * Created on 4 February, 2025, 4:15 PM
 */


#include <xc.h>
#include "clcd.h"
#include "main1.h"
#include "eeprom.h"
#include "external_eeprom.h"
#include "black_box.h"
#include "i2c.h"
#include "ds1307.h"
#include "UT1.h"

extern unsigned int i, key, count, d;
unsigned char event[9][3] = {"ON", "GN", "G1", "G2", "G3", "G4", "G5", "GR", "C-"};
unsigned char menu[4][17] = {"VIEW LOG", "DOWNLOAD LOG", "CLEAR LOG", "SET TIME"};
unsigned char time[9] = "00:00:00";
unsigned char s_time[9] = "00:00:00";
unsigned int hr = 0, min = 0, sec = 0, v = 0, extra = 0;
volatile unsigned long int wait = 0;
unsigned int speed, value, j = 0, d = 1, k, star = 0, ind = 0, once = 1, c, flag = 1;
char prom[10][17];
static unsigned int add=0,add1 = 0,press;// EEPROM address and press count
void view_dashboard(void) {
    count=0;
    star=0;
    c=0;
    ind=0;
    d = 1;
    unsigned char a[17] = "                ";
    clcd_print("  TIME   EV  SP ", LINE1(0));// Display labels on the screen
    clcd_print(time, LINE2(0));// Display current time
    value = read_Adc(); // Read ADC value for speed
    speed = value / 10.33;// Convert ADC value to speed
    clcd_putch((speed / 10) + 48, LINE2(13));
    clcd_putch((speed % 10) + 48, LINE2(14));
    if (i == 8) {
        if (key == '2' || key == '3') {
            i = 1;
            event_store();// Store event if key 1 is pressed
        }
    }
    else  if (key == '1') {
        i = 8;
         event_store();
    }
    else if (key == '2') {
        if (i >= 0 && i < 7)
            i++;              // Increment event
        event_store();
    } else if (key == '3') {
        if (i > 1 && i <= 7)
            i--;             // decrement event
        event_store();
    }

    clcd_print(event[i], LINE2(9));// Display the current event on the screen

}

// Function to store the event details (time, event, and speed) in EEPROM
void event_store() {
    if (press++ > 9) {
        press = 10;//prevent overflows
        if (extra++ > 9) {
            extra = 0; 
        }
    }
    if (add == 100) // Reset EEPROM address after 100 entries
        add = 0;    
    for (j = 0; j < 8; j++) {
        if (j == 2 || j == 5)
            continue;  // Skip ':' in time format
        write_external_eeprom(add++, time[j]); // Store time
    }
    for (k = 0; k < 2; k++)
        write_external_eeprom(add++, event[i][k]);//store event
        write_external_eeprom(add++, (speed / 10) + 48);
        write_external_eeprom(add++, (speed % 10) + 48);
    //for(wait=300;wait--;);  
}

// Function to display the main menu and handle navigation
void display_main_menu() {

    if (star == 0) {
        clcd_putch('*', LINE1(0)); // Display cursor on the first menu option
        if (count == 1) {
            if (key == '#' && ind == 1)
                flag = 1;
        }
        if (count == 2) {
            if (key == '#' && ind == 0) {
                once = 1;
                CLEAR_DISP_SCREEN;
                state = e_view_log;
            } else if (key == '#' && ind == 1) {
                CLEAR_DISP_SCREEN;
                state = e_download_log;
            } else if (key == '#' && ind == 2) {
                CLEAR_DISP_SCREEN;
                state = e_clear_log;
            }
            count = 0;
        }
    } else if (star == 1) {
        clcd_putch('*', LINE2(0));// Display cursor on the second menu option
        if (count == 1) {
            if (key == '#' && ind == 0)
                flag = 1;
        }
        if (count == 2) {
            if (key == '#' && ind == 0) {
                state = e_download_log;
            } else if (key == '#' && ind == 1) {
                state = e_clear_log;
            } else if (key == '#' && ind == 2) {
                d = 1;
                CLEAR_DISP_SCREEN;
                state = e_set_time;
            }
            count = 0;
        }
    }

    if (key == '1') {
        CLEAR_DISP_SCREEN;
        if (star == 0)
            star = 1;
        else if (star == 1 && ind < 2)
        {
            ind++;
            
        }
    }
    if (key == '2') {
        CLEAR_DISP_SCREEN;
        if (star == 1)
            star = 0;
        else if (star == 0 && ind > 0)
        {
            ind--;
       
        }
    }
    clcd_print(menu[ind], LINE1(2)); // Display the selected menu option
    clcd_print(menu[ind + 1], LINE2(2));// Display the next menu option
}

// Function to view the logs stored in EEPROM
void view_log() {
    if (press == 0) {
           CLEAR_DISP_SCREEN;
        clcd_print("   LOG EMPTY   ", LINE1(0));//Display message if log is empty
    } else {
        clcd_print("IND TIME   EV SP", LINE1(0));
        if (once == 1) {
            event_reader();// Read events from EEPROM
            once = 0;
        }
     // Handle log navigation using key presses

        if (key == '1') {
            if (c >= 0 && c < press - 1) {
                c++;// Move down in the log

                CLEAR_DISP_SCREEN;
            }
        }
        else if (key == '2') {
            if (c > 0 && c <= press - 1) {
                c--; // Move up in the log

                CLEAR_DISP_SCREEN;
            }
        }

        clcd_putch(c + 48, LINE2(0));// Display current index

        clcd_print(prom[c], LINE2(2));// Display the log event

    }

}
// Function to read events from EEPROM into the prom array

void event_reader() {
    
    unsigned int b;
    add1 = extra * 10;// Set start address for reading events
    for (b = 0; b < press; b++) {
        for (j = 0; j < 15; j++) {
            if (j == 8 || j == 11)
                prom[b][j] = ' ';

            else if (j == 2 || j == 5)
                prom[b][j] = ':';

            else if (j == 14)
                prom[b][j] = '\0';

            else
                prom[b][j] = read_external_eeprom(add1++);// Read from EEPROM

        }
        if (add1 == 100)// Wrap around address
            add1 = 0;

    }
}

// Function to clear the event log
void clear_log() {
    CLEAR_DISP_SCREEN;
    press = 0;// Reset event press count
    add=0;// Reset EEPROM address
    clcd_print(" LOG CLEARED ", LINE1(2));
    clcd_print(" SUCCESSFULLY ", LINE2(2));
}
// Function to download the event log via UART
void download_log() {
     CLEAR_DISP_SCREEN;
    unsigned int c;
    init_uart();
    if (press == 0) {
        clcd_print("LOG EMPTY", LINE1(2));//Display message if log is empty
    } else {
        clcd_print("DOWNLOAD", LINE1(2));
        clcd_print("SUCCESSFULLY", LINE2(2));
        event_reader();// Read events from EEPROM
        if (flag == 1) {
            puts("Downloaded logs\n\r");// Notify log download
            for (c = 0; c < press; c++) {
                puts(prom[c]);// Print each event entry
                puts("\n\r");
            }
            flag = 0;
        }
    }
}
// Function to set the time
void set_time() {
    CLEAR_DISP_SCREEN;
    clcd_print("HH:MM:SS", LINE1(4));// Display time format 

    if (d == 1) {
        hr = time[0] - '0';// Extract hours from time buffer
        hr = hr * 10 + time[1] - '0';// Combine hours digits

        min = time[3] - '0'; // Extract minutes from time buffer
        min = min * 10 + time[4] - '0'; // Combine minutes digits

        sec = time[6] - '0';// Extract seconds from time buffer
        sec = sec * 10 + time[7] - '0';// Combine seconds digits
        d = 0;
    }
    if (key == '2') {
        v++; // Cycle through hour, minute, second field
        if (v == 3)
            v = 0;
    }
    if (key == '1') {
        if (v == 0) {
            hr++;//increment hour
            if (hr == 24)
                hr = 0;
        } else if (v == 1) {
            min++;//increment minute
            if (min == 60)
                min = 0;
        } else if (v == 2) {
            sec++;//increment second
            if (sec == 60)
                sec = 0;
        }
    }
    //display box on the field being selected 
    if (v == 0 && wait++ <= 100) {
        clcd_putch(0xff, LINE2(4));
        clcd_putch(0xff, LINE2(5));
    } else if (v == 1 && wait++ <= 100) {
        clcd_putch(0xff, LINE2(7));
        clcd_putch(0xff, LINE2(8));
    } else if (v == 2 && wait++ <= 100) {
        clcd_putch(0xff, LINE2(10));
        clcd_putch(0xff, LINE2(11));
    } else if (wait < 200) {
        clcd_print(s_time, LINE2(4));
    } else
        wait = 0;

    
    //update the set hour, minute and second back to display updated time
    s_time[0] = hr / 10 + '0';
    s_time[1] = hr % 10 + '0';
    s_time[3] = min / 10 + '0';
    s_time[4] = min % 10 + '0';
    s_time[6] = sec / 10 + '0';
    s_time[7] = sec % 10 + '0';
    clcd_print(s_time, LINE2(4)); // Display updated time

    //if key11 is pressed Save time to DS1307 and go back to dashboard

    if (key == '#' && d == 0) {
        write_ds1307(HOUR_ADDR, ((hr / 10) << 4 | hr % 10));
        write_ds1307(MIN_ADDR, ((min / 10) << 4 | min % 10));
        write_ds1307(SEC_ADDR, ((sec / 10) << 4 | sec % 10));
        state = e_dashboard;
    }

}



