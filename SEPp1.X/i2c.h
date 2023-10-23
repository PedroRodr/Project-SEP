/* 
 * File:   i2c.h
 * Authors: pedro, mariana, antonia
 *
 * Created on October 23, 2023
 */

#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <p24FJ256GA702.h>
#include <libpic30.h>

#define I2C_READ 1
#define I2C_WRITE 0

void I2C_ACK(void);
void I2C_NACK(void);
void initializeI2C();
void startConditionI2C();
void restartConditionI2C();
void stopConditionI2C();
uint8_t writeByteI2C(uint8_t data);
uint8_t readByteI2C(unsigned char ACK_NACK);
void readRegisterData(uint8_t registerRead);

#endif // I2C_H
