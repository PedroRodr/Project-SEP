/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*******************************************************************************/
#define FCY 16000000

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "bsp/adc.h"
#include "bsp/lcd.h"
#include "bsp/timer_1ms.h"
#include "bsp/buttons.h"
#include "bsp/leds.h"
#include "bsp/rtcc.h"
#include <libpic30.h>
 
#include "io_mapping.h"

uint8_t buff;

void configureSPI() 
{
    SPI1CON1bits.SPIEN = 0; // Disable the SPI peripheral during configuration
    IFS0bits.SPI1IF = 0; // Clear the SPI peripheral interrupt flag
    IEC0bits.SPI1IE = 0; // Disable the SPI peripheral interrupt flag
    SPI1BUFL = 0;
    SPI1CON1bits.ENHBUF=0; 
    SPI1CON1bits.DISSCK = 0; // Enable generation of SCK signal
    SPI1CON1bits.DISSDO = 0; // Enable generation of SDO signal
    SPI1CON1bits.MODE16 = 1; // Set 16-bit mode
    SPI1CON1bits.SMP = 0; // Input data sampled at mid-bit
    SPI1CON1bits.CKP = 0; // Idle is SCK low
    SPI1CON1bits.CKE = 1; // Data changes on SCK falling edge
    SPI1BRGL = 0x80; // Baud rate divisor FCY/100k
    SPI1CON1bits.MSTEN = 1; // Set for master mode
    //SPI1CON2 = 0; // Fully disable frame mode    
     SPI1CON1bits.SPIEN = 1; // enable SPI port, clear status
    
     
    RPINR20bits.SDI1R  = 26; // MISO
    TRISGbits.TRISG7 = 1;
    ANSGbits.ANSG7 = 0;
    
    RPOR9bits.RP19R  = 7;    // MOSI
    TRISGbits.TRISG8 = 0;
    ANSGbits.ANSG8 = 0;
    
    RPOR10bits.RP21R  = 8;    // SPI Clock (SCK1) pin
    TRISGbits.TRISG6 = 0;
    ANSGbits.ANSG6 = 0;
    
    RPOR13bits.RP27R  = 9;    // CS
    TRISGbits.TRISG9 = 0;
    ANSGbits.ANSG9 = 0;
    LATGbits.LATG9 = 1;

    
}


uint8_t writeByteSPI(uint8_t dataOut) 
{
    SPI1BUFL = dataOut; // write to buffer for TX
    while(!SPI1STATLbits.SPIRBF); // wait for transfer to complete
    buff = SPI1BUFL; // read the received value
    return buff;
}

void uartInit(){
    
    U1BRG = 103; // PIC has 16 MHz: U1BRG = (16000000)/(16*9600) - 1  = 103

    U1MODEbits.BRGH = 0; // Low-speed mode (use 16x baud clock)
    U1STAbits.UTXEN = 1; // Enable transmitter
    U1MODEbits.PDSEL = 0; // 8-bit data, no parity
    U1MODEbits.STSEL = 0; // 1 stop bit
    U1MODEbits.UEN = 0; // only U1TX and U1RX
    U1MODEbits.BRGH = 0; // Low-speed mode (use 16x baud clock)
    U1STAbits.UTXEN = 1; // Enable transmitter

    U1STAbits.UTXISEL1 = 0; // Interruptions
    U1STAbits.UTXISEL0 = 0; // Interruptions
    
    //U1TX pin
    RPINR18bits.U1RXR = 10; // U1RX on RP10
    RPOR8bits.RP17R = 3; // U1TX on RP2

    U1MODEbits.UARTEN = 1; // enable UART

    
    
    
}

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables and Functions
// *****************************************************************************
// *****************************************************************************
extern void SYS_Initialize ( void ) ;
static void BlinkAliveEventHandler( void );
static void ScreenUpdateEventHandler( void );

static RTCC_DATETIME time;
static RTCC_DATETIME lastTime = {0};
static volatile bool toggleBlinkAlive = false;
static volatile bool allowScreenUpdate = true;

#define MEMCMP_VALUES_IDENTICAL 0
#define DUMMY 0xAA

#define AX 0x01





// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
int main ( void )
{
    uint16_t adcResult;
    uint16_t lastAdcResult = 0xFFFF;

    uint16_t datain[2];
    uint32_t value;
    uint32_t datain_0;
    uint32_t datain_1;
    
    float data;
    
    /* Call the System Initialize routine*/
    SYS_Initialize ( );
    configureSPI();
    uartInit();
    
    /* To determine how the LED and Buttons are mapped to the actual board
     * features, please see io_mapping.h. */
    LED_Enable ( LED_BLINK_ALIVE );
    LED_Enable ( LED_BUTTON_PRESSED );

    BUTTON_Enable ( BUTTON_DEMO );
    
    /* Get a timer event once every 100ms for the blink alive. */
    TIMER_SetConfiguration ( TIMER_CONFIGURATION_1MS );
    TIMER_RequestTick( &BlinkAliveEventHandler, 500 );
    TIMER_RequestTick( &ScreenUpdateEventHandler, 170 );
    
    /* The TIMER_1MS configuration should come before the RTCC initialization as
     * there are some processor modules that require the TIMER_1MS module to be
     * configured before the RTCC module, as the RTCC module is emulated using
     * the TIMER_1MS module. */
    time.bcdFormat = false;
    lastTime.bcdFormat = false;
    RTCC_BuildTimeGet( &time );
    RTCC_Initialize( &time );
    memset(&lastTime,0,sizeof(lastTime)); 

    ADC_SetConfiguration ( ADC_CONFIGURATION_DEFAULT );
    ADC_ChannelEnable ( ADC_CHANNEL_POTENTIOMETER );
    
    /* Clear the screen */
    printf( "\f" );   
    
    while ( 1 )
    {
        adcResult = ADC_Read10bit( ADC_CHANNEL_POTENTIOMETER );

        RTCC_TimeGet( &time );
        
        
        
        //Only print if the ADC value or time has changed since the last time 
        // around the loop and we haven't updated recently too recently.
        if(allowScreenUpdate == true)
        {
            if( (adcResult != lastAdcResult) ||
                (memcmp(&time, &lastTime, sizeof(time)) != MEMCMP_VALUES_IDENTICAL) )
            {                  
                //Either ADC or time has changed, and the screen update refresh
                // limit has expired, update the screen.
                allowScreenUpdate = false;
                
//                printf( "Time %02d:%02d:%02d   Pot = %4d\r\n", 
//                        time.hour, 
//                        time.minute, 
//                        time.second, 
//                        adcResult
//                      );
               printf("AX: %f\r", data);
                
                lastAdcResult = adcResult;
                memcpy(&lastTime, &time, sizeof(time));
            }
        }

        if(toggleBlinkAlive == true)
        {
            LED_Toggle( LED_BLINK_ALIVE );
            toggleBlinkAlive = false;
        }
        
        /* To determine how the LED and Buttons are mapped to the actual board
         * features, please see io_mapping.h. */
        if(BUTTON_IsPressed( BUTTON_DEMO ) == true)
        {
            LED_On( LED_BUTTON_PRESSED );
            
            // UART TEST
            U1TXREG = 'S';
            U1TXREG = 'E';
            U1TXREG = 'P';
            
            LATGbits.LATG9 = 0;
        
            __delay_ms(10);

             writeByteSPI(AX);
             writeByteSPI(DUMMY);
             datain[0] = writeByteSPI(DUMMY);
     //        datain[1] = writeByteSPI(DUMMY);

             datain_0 = (uint32_t*)&datain[0]; 
     //        datain_1 = (uint32_t*)&datain[1]; 

             //value = (datain_0 << 16) | datain_1;

             data = *((float*)&datain_0);
             
            __delay_ms(10); 

             LATGbits.LATG9 = 1;

        }
        else
        {
            LED_Off( LED_BUTTON_PRESSED );
        }
    }
}

static void BlinkAliveEventHandler(void)
{    
    toggleBlinkAlive = true;
}

static void ScreenUpdateEventHandler(void)
{
    allowScreenUpdate = true;
}