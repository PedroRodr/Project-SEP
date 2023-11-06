/* 
 * File:   main.c
 * Authors: pedro, mariana, antonia
 *
 * Created on September 15, 2023, 4:31 PM
 */


// PIC24FJ256GA702 Configuration Bit Settings

// 'C' source line config statements

// FSEC
#pragma config BWRP = OFF               // Boot Segment Write-Protect bit (Boot Segment may be written)
#pragma config BSS = DISABLED           // Boot Segment Code-Protect Level bits (No Protection (other than BWRP))
#pragma config BSEN = OFF               // Boot Segment Control bit (No Boot Segment)
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = DISABLED           // General Segment Code-Protect Level bits (No Protection (other than GWRP))
#pragma config CWRP = OFF               // Configuration Segment Write-Protect bit (Configuration Segment may be written)
#pragma config CSS = DISABLED           // Configuration Segment Code-Protect Level bits (No Protection (other than CWRP))
#pragma config AIVTDIS = OFF            // Alternate Interrupt Vector Table bit (Disabled AIVT)

// FBSLIM
#pragma config BSLIM = 0x1FFF           // Boot Segment Flash Page Address Limit bits (Enter Hexadecimal value)

// FOSCSEL
#pragma config FNOSC = PRIPLL           // Oscillator Source Selection (Primary Oscillator with PLL module (XT + PLL, HS + PLL, EC + PLL))
#pragma config PLLMODE = DISABLED       // PLL Mode Selection (No PLL used; PLLEN bit is not available)
#pragma config IESO = ON                // Two-speed Oscillator Start-up Enable bit (Start up device with FRC, then switch to user-selected oscillator source)

// FOSC
#pragma config POSCMD = HS              // Primary Oscillator Mode Select bits (HS Crystal Oscillator Mode)
#pragma config OSCIOFCN = ON            // OSC2 Pin Function bit (OSC2 is clock output)
#pragma config SOSCSEL = ON             // SOSC Power Selection Configuration bits (SOSC is used in crystal (SOSCI/SOSCO) mode)
#pragma config PLLSS = PLL_PRI          // PLL Secondary Selection Configuration bit (PLL is fed by the Primary oscillator)
#pragma config IOL1WAY = ON             // Peripheral pin select configuration bit (Allow only one reconfiguration)
#pragma config FCKSM = CSDCMD           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler bits (1:32,768)
#pragma config FWPSA = PR128            // Watchdog Timer Prescaler bit (1:128)
#pragma config FWDTEN = ON              // Watchdog Timer Enable bits (WDT Enabled)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config WDTWIN = WIN25           // Watchdog Timer Window Select bits (WDT Window is 25% of WDT period)
#pragma config WDTCMX = WDTCLK          // WDT MUX Source Select bits (WDT clock source is determined by the WDTCLK Configuration bits)
#pragma config WDTCLK = LPRC            // WDT Clock Source Select bits (WDT uses LPRC)

// FPOR
#pragma config BOREN = ON               // Brown Out Enable bit (Brown Out Enable Bit)
#pragma config LPCFG = OFF              // Low power regulator control (No Retention Sleep)
#pragma config DNVPEN = ENABLE          // Downside Voltage Protection Enable bit (Downside protection enabled using ZPBOR when BOR is inactive)

// FICD
#pragma config ICS = PGD2               // ICD Communication Channel Select bits (Communicate on PGEC2 and PGED2)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FDEVOPT1
#pragma config ALTCMPI = DISABLE        // Alternate Comparator Input Enable bit (C1INC, C2INC, and C3INC are on their standard pin locations)
#pragma config TMPRPIN = OFF            // Tamper Pin Enable bit (TMPRN pin function is disabled)
#pragma config SOSCHP = ON              // SOSC High Power Enable bit (valid only when SOSCSEL = 1 (Enable SOSC high power mode (default))
#pragma config ALTI2C1 = ALTI2CEN       // Alternate I2C pin Location (SDA1 and SCL1 on RB9 and RB8)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//#define FOSC 8000000
#define FCY 16000000

#include <xc.h>
#include <p24FJ256GA702.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libpic30.h>
#include "mpu.h"
#include "i2c.h"
#include "spi.h"
#include "ioc.h"

#define TRUE 1
#define FALSE 0

#define PRIORITY 3

volatile uint16_t iocFlag;
uint8_t T1_expired;

int main(int argc, char** argv) 
{

    //uint8_t dataout = 0x3D;
    uint8_t datain;
    
    
    initializeI2C();
    initializeMPU6050();
    configureSPISlave();
    initializeInterruption();
    timerConfig();
    
//    ANSAbits.ANSA0 = 0; // Unset the pin ANS bit, make sure analog functionality is disabled
//    LATAbits.LATA0 = 1; // Set the pin LAT bit, set outputs to 1
//    TRISAbits.TRISA0 = 0; // Unset the pin TRIS bit, enables the digital output
//    
//    
//    ANSBbits.ANSB1 = 0; // Unset the pin ANS bit, make sure analog functionality is disabled
//    LATBbits.LATB1 = 1; // Set the pin LAT bit, set outputs to 1
//    TRISBbits.TRISB1 = 0; // Unset the pin TRIS bit, enables the digital output
    
    while(1) {
        
        if(iocFlag) {
            
            sendCommand();
            iocFlag = 0;
            
        }

//        if (T1_expired == 1){
//            MPU6050();
//            IFS0bits.T1IF = 0; //Reset Timer2 interrupt flag and Return from ISR
// 
//            T1_expired = 0;
//       
//        } 
        
        // Perform your MPU6050 data reading here
        //MPU6050();  // Reads data from MPU6050

        // Delay for a short duration (adjust this as needed)
        __delay_ms(10);



// - - - - - - - LED - - - - - - - - -
//        LATAbits.LATA0 = 0;
//        __delay_ms(100);
//        LATAbits.LATA0 = 1;
//        __delay_ms(100);*/
    }
    
    return (EXIT_SUCCESS);
}