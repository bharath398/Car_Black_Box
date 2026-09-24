#include <xc.h>
#include "main1.h"
void init()
{
    CHS3=0;
    CHS2=1;
    CHS1=0;
    CHS0=0;
    
    ADON=1;
    
    VCFG1=0;
    VCFG0=0;
    
    PCFG3=1;
    PCFG2=0;
    PCFG1=1;
    PCFG0=0;
    
    ADFM=1;
    ACQT2=0;
    ACQT1=0;
    ACQT0=1;
    
    ADCS2=0;
    ADCS1=1;
    ADCS0=0;
    
 }
unsigned int read_Adc()
{
    unsigned int res=0;
    GO=1;
    while(GO);
    res=(ADRESH<<8)|ADRESL;
    return res;
    
}
