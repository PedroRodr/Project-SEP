/* 
 * File:   ioc.c
 * Authors: pedro, mariana, antonia
 *
 * Created on October 23, 2023
 */

#include <libpic30.h>
#include "ioc.h"

volatile uint16_t iocFlag = 0; // Shared flag, volatile attribute prevents dangerous optimization

void initializeInterruption(){
    
    PADCONbits.IOCON = 1; // Enables the IoC functionality for all pins
    //ANSBbits.ANSB6 = 0; // Unset the pin ANS bit, make sure analog functionality is disabled
    TRISBbits.TRISB6 = 1; // Set the pin TRIS bit, enables the digital input
    IOCPBbits.IOCPB6 = 0; // Disables rising edge detection
    IOCNBbits.IOCNB6 = 1; // Enables falling edge detection
    IOCFBbits.IOCFB6 = 0; // Clear the individual interrupt flag for RA1
    IFS1bits.IOCIF = 0; // Clear overall IoC interrupt flag
    IPC4bits.IOCIP = 1; // Configure the IoC interrupts priority to value 1
    IEC1bits.IOCIE = 1; // Enable the IoC interrupts
    
}
