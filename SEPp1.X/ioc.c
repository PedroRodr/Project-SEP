/* 
 * File:   ioc.c
 * Authors: pedro, mariana, antonia
 *
 * Created on October 23, 2023
 */

#include <libpic30.h>
#include "ioc.h"
#include "spi.h"
#include "mpu.h"

extern volatile uint16_t iocFlag; // Shared flag, volatile attribute prevents dangerous optimization
extern uint8_t T1_expired;

extern float ax, ay, az, gx, gy, gz, ti;


void initializeInterruption(){
    PADCONbits.IOCON = 1; // Enables the IoC functionality for all pins
    ANSBbits.ANSB0 = 0; // Unset the pin ANS bit, make sure analog functionality is disabled
    TRISBbits.TRISB0 = 1; // Set the pin TRIS bit, enables the digital input
    IOCPBbits.IOCPB0 = 0; // Disables rising edge detection
    IOCNBbits.IOCNB0 = 1; // Enables falling edge detection
    IOCFBbits.IOCFB0 = 0; // Clear the individual interrupt flag for RA1
    IFS1bits.IOCIF = 0; // Clear overall IoC interrupt flag
    IPC4bits.IOCIP = 1; // Configure the IoC interrupts priority to value 1
    IEC1bits.IOCIE = 1; // Enable the IoC interrupts
    INTCON2bits.GIE = 1;  //Enables Global Interrupt
    
}

void timerConfig(){
    T1CON = 0x00; //Stops the Timer2 and reset control reg.
    TMR1 = 0x00; //Clear contents of the timer register
    PR1 = 7812; //Load the Period register with the value to 1 second(7812)
    IPC0bits.T1IP = 0x02; //Setup Timer2 interrupt for desired priority level
    // (this example assigns level 1 priority)
    IFS0bits.T1IF = 0; //Clear the Timer2 interrupt status flag
    IEC0bits.T1IE = 1; //Enable Timer2 interrupts
    T1CONbits.TGATE = 1; //Set up Timer2 for operation in Gated
    //Time Accumulation mode
    T1CONbits.TGATE = 0;
    T1CONbits.TCKPS = 3; // Set prescaler 1:256
    T1CONbits.TON = 1; //Start Timer2 
}


void __attribute__((__interrupt__, auto_psv)) _IOCInterrupt(void){
    
//    if (IOCFBbits.IOCFB0 == 1)
//    {               
    iocFlag = 1;
    IFS1bits.IOCIF = 0;
    }

void __attribute__((__interrupt__, auto_psv)) _T1Interrupt( void ){
    IFS0bits.T1IF = 0; // Clear IoC interrupt flag
    T1_expired = 1; 
    
}