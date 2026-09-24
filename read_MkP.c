/*
 * File:   read_MkP.c
 * Author: Bharath
 *
 * Created on 5 February, 2025, 12:01 PM
 */


#include <xc.h>
#include "main1.h"
void init_Mkp()
{
    TRISB=(TRISB & 0x01)|0x1E;
    TRISB0=0;
    RBPU=0;
}
unsigned char Read_Mkp()
{
    R1=0;R2=1;R3=1;
    if(C1==0)
    return '1';
    else if(C2==0)
    return '4';
    R1=1;R2=0;R3=1;
    if(C1==0)
    return '2';
    else if(C4==0)
    return '#';
    R1=1;R2=1;R3=0;
    R1=1;R2=1;R3=0;
    if(C1==0)
    return '3';
    else if(C4==0)
    return '0';
    
    return 0xff;

}
unsigned char read_Mkp_Switch(unsigned char detection_type)
{
static int once=1;
unsigned char key=0xFF;
key=Read_Mkp();
if(detection_type == STATE_CHANGE)
{
    if(key!=0xFF && once)
    {
        once=0;
        return key;
    }
    else if(key==0xFF)
    {
        once=1;
    }
}
else if(detection_type == LEVEL_CHANGE)
{
    return key;
}
return 0xFF;
}
