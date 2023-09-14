/*****************************************************************************/
/*****************************************************************************/

#include "at89c5131.h"
#include <stdio.h>
#include "I2C.h"
void I2C_SCL_pulse(void);
void I2C_delay_5us(unsigned int multiplo_5us);

void I2C_start(void){
	SDA = 1;
	SCL = 1;
	I2C_delay_5us(2);
	SDA = 0;
	I2C_delay_5us(2);
	SCL = 0;
}
	
void I2C_stop(void) {
	SDA = 0;
	SCL = 1;
	I2C_delay_5us(2);
	SDA = 1;
	I2C_delay_5us(2);
	SCL = 0;
}

void I2C_write(unsigned char byte) {
	unsigned int i;
	for (i = 0; i < 8; i++) {
		if ((byte&0x80) == 0)
			SDA = 0;
		else 
			SDA = 1;
		I2C_SCL_pulse();
		byte = byte<<1;
	}
	I2C_NACK();
}

unsigned char I2C_read(void) {
	unsigned int i;
	unsigned char dato;
	for (i = 0; i < 8; i++) {
		SDA = 1;
		SCL = 0;
		I2C_delay_5us(2);
		SCL = 1;
		I2C_delay_5us(1);
		if (SDA == 1)
			dato = dato | 0x01;
		I2C_delay_5us(1);
		SCL = 0;
		if (i < 7)
		dato = dato<<1;
	}
	return(dato);
}

void I2C_ACK(void) {
	SDA = 0;
	I2C_SCL_pulse();
}

void I2C_NACK(void) {
	SDA = 1;
	I2C_SCL_pulse();
}

void I2C_SCL_pulse(void) {
	SCL = 0;
	I2C_delay_5us(2);
	SCL = 1;
	I2C_delay_5us(2);
	SCL = 0;
}

void I2C_delay_5us(unsigned int multiplo_5us) {
	unsigned int i,j;
	for(i=0; i < multiplo_5us; i++)
		j++;
}