/* 
 * File:   spi.c
 * Authors: pedro, mariana, antonia
 *
 * Created on October 23, 2023
 */

#include <libpic30.h>
#include "spi.h"
#include "mpu.h"


extern uint8_t dataOut, dataIn;
float ax, ay, az, gx, gy, gz, ti;


void configureSPISlave(){
    
    SPI1CON1bits.SPIEN = 0; // Disable the SPI peripheral during configuration
    IFS0bits.SPI1IF = 0; // Clear the SPI peripheral interrupt flag
    IEC0bits.SPI1IE = 0; // Disable the SPI peripheral interrupt flag
    SPI1CON1bits.ENHBUF=0;
    SPI1STATLbits.SPIROV=0;
    //SPI1CON1bits.DISSCK = 0; // Enable generation of SCK signal
    SPI1CON1bits.DISSDO = 0; // Enable generation of SDO signal
    SPI1CON1bits.MODE16 = 1; // Set 16-bit mode
    SPI1CON1bits.SMP = 0; // Input data sampled at mid-bit
    SPI1CON1bits.CKP = 0; // Idle is SCK low
    SPI1CON1bits.CKE = 1; // Data changes on SCK falling edge
    SPI1CON1bits.MSTEN = 0; // Set for slave mode
    SPI1CON1bits.SSEN = 1; // Set for slave mode
    SPI1CON2 = 0; // Fully disable frame mode
    
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

// FUNCAO ANTIGA
//uint8_t writeByteSPI(uint8_t dataOut{  
//    
//    SPI1BUFL = dataOut; // write to buffer for TX
//    while(!SPI1STATLbits.SPIRBF); // wait for transfer to complete
//    return SPI1BUFL; // read the received value
//    while(SPI1STATLbits.SPITBF); // wait for transfer to complete
//    
//}


void sendCommand(){
    
        uint16_t command;
                  
        uint16_t buffer;
        uint16_t dataOut[2];
        uint16_t* parameterHex;
        float parameter; // value after MPU has been processed
        
        SPI1BUFL = DUMMY; 
        while(!SPI1STATLbits.SPIRBF); // wait for transfer to complete
        command = SPI1BUFL; // command, what we're going to receive
        while(SPI1STATLbits.SPITBF); // wait for transfer to complete
        
        
        SPI1BUFL = DUMMY;
        while(!SPI1STATLbits.SPIRBF); // wait for transfer to complete
        buffer = SPI1BUFL; // DUMMY
        while(SPI1STATLbits.SPITBF); // wait for transfer to complete
        
        
             if (command == AX){parameter = ax;}
        else if (command == AY){parameter = ay;}    
        else if (command == AZ){parameter = az;}   
        else if (command == GX){parameter = gx;}       
        else if (command == GY){parameter = gy;}       
        else if (command == GZ){parameter = gz;}
        else if (command == TI){parameter = ti;}
        else {return;} // command will never be DUMMY
        
        
        // converts from float to hex
      //  parameterHex = (uint32_t*)&parameter;    
      //  dataOut[0] = (uint16_t)(*parameterHex);
      //  dataOut[1] = (uint16_t)(*parameterHex >> 16);    
        
        
        // sends the data in two parts
        dataOut[1]=10;
        SPI1BUFL = dataOut[1];
        while(!SPI1STATLbits.SPIRBF); // wait for transfer to complete
        buffer = SPI1BUFL; // DUMMY
        while(SPI1STATLbits.SPITBF); // wait for transfer to complete
 
//        SPI2BUFL = dataOut[0];
//        while(!SPI2STATLbits.SPIRBF); // wait for transfer to complete
//        buffer = SPI2BUFL; // DUMMY
//        while(SPI2STATLbits.SPITBF); // wait for transfer to complete

}