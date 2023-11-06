/* 
 * File:   spi.h
 * Authors: pedro, mariana, antonia
 *
 * Created on October 23, 2023
 */

#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <p24FJ256GA702.h>
#include <libpic30.h>

#define SPI_SDI_PIN  RPINR20bits.SDI1R   // SPI Data Input (SDI) pin
#define SPI_SDO_PIN  RPOR3bits.RP7R      // SPI Data Output (SDO1) pin
#define SPI_SCK_PIN  RPOR4bits.RP8R      // SPI Clock (SCK1) pin

#define DUMMY 0xAA
#define AX 0x01
#define AY 0x02
#define AZ 0x03
#define GX 0x04
#define GY 0x05
#define GZ 0x06
#define TI 0x07

void configureSPISlave();
void writeByteSPI(float sensorValue);
void sendCommand();


#endif // SPI_H
