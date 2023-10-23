/* 
 * File:   spi.c
 * Authors: pedro, mariana, antonia
 *
 * Created on October 23, 2023
 */

#include <libpic30.h>
#include "spi.h"


void configureSPISlave(){
    
    SPI1CON1bits.SPIEN = 0; // Disable the SPI peripheral during configuration
    IFS0bits.SPI1IF = 0; // Clear the SPI peripheral interrupt flag
    IEC0bits.SPI1IE = 0; // Disable the SPI peripheral interrupt flag
    SPI1CON1bits.DISSCK = 0; // Enable generation of SCK signal
    SPI1CON1bits.DISSDO = 0; // Enable generation of SDO signal
    SPI1CON1bits.MODE16 = 0; // Set 8-bit mode
    SPI1CON1bits.SMP = 0; // Input data sampled at mid-bit
    SPI1CON1bits.CKP = 0; // Idle is SCK low
    SPI1CON1bits.CKE = 1; // Data changes on SCK falling edge
    SPI1CON1bits.MSTEN = 0; // Set for slave mode
    SPI1CON1bits.SSEN = 1; // Set for slave mode
    SPI1CON2 = 0; // Fully disable frame mode
    SPI1BUFL = 0;
    
    RPINR20bits.SDI1R = 1;       //SPI SLAVE Data Input (SDI1) pin 5
    TRISBbits.TRISB1 = 1;
    ANSBbits.ANSB1 = 0;
    
    RPOR1bits.RP2R = 7;         //SPI SLAVE Data Output (SDO1) pin 6
    TRISBbits.TRISB2 = 0;
    ANSBbits.ANSB2 = 0;
    
    RPINR20bits.SCK1R = 3;       //SPI SLAVE Clock Input (SCK1IN) pin 7
    TRISBbits.TRISB3 = 1;   
    ANSBbits.ANSB3 = 0;    
    
    RPINR21bits.SS1R = 0;       //SPI SLAVE Select Input (SS1IN) pin 4
    TRISBbits.TRISB0 = 1;
    ANSBbits.ANSB0 = 0;    
    
    SPI1CON1bits.SPIEN = 1;     // enable SPI port, clear status
    
}


void writeByteSPI(uint8_t dataOut, uint8_t dataIn){  
    
    SPI1BUFL = dataOut; // write to buffer for TX
    while(!SPI1STATLbits.SPIRBF); // wait for transfer to complete
    dataIn = SPI1BUFL; // read the received value
    
}

