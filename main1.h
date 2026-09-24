/* 
 * File:   main.h
 * Author: Bharath
 *
 * Created on 5 February, 2025, 6:16 AM
 */

#ifndef MKP_H
#define	MKP_H


#define C1 RB1 
#define C2 RB2 
#define C3 RB3
#define C4 RB4 
#define R1 RB5
#define R2 RB6
#define R3 RB7 

#define STATE_CHANGE 1
#define LEVEL_CHANGE 0
void init_Mkp();
unsigned char Read_Mkp();
unsigned char read_Mkp_Switch(unsigned char detection_type);
unsigned int read_Adc();
void init();


#endif	/* MKP_H */



