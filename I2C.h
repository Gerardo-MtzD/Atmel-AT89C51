/*****************************************************************************/
/*****************************************************************************/
#include "at89c5131.h"

sbit SCL = P4^0; //Select your ports according to connections
sbit SDA = P4^1;

void I2C_start(void);


void I2C_stop(void);


void I2C_write(unsigned char byte);


unsigned char I2C_read(void);


void I2C_ACK(void);


void I2C_NACK(void);

void I2C_SCL_pulse(void);