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

void configureSPISlave();
void writeByteSPI(uint8_t dataOut, uint8_t dataIn);

#endif // SPI_H
