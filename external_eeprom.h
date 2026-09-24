/* 
 * File:   external_eeprom.h
 * Author: Bharath
 *
 * Created on 8 February, 2025, 9:29 AM
 */

#ifndef EXTERNAL_EEPROM_H
#define	EXTERNAL_EEPROM_H


#define SLAVE_READ		0xA1
#define SLAVE_WRITE		0xA0
void write_external_eeprom(unsigned char address1,  unsigned char data);
unsigned char read_external_eeprom(unsigned char address1);


#endif	/* EXTERNAL_EEPROM_H */

