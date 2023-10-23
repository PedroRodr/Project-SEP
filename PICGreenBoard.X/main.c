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
 
#include "io_mapping.h"



void configureSPI() 
{
    SPI1CON1bits.SPIEN = 0; // Disable the SPI peripheral during configuration
    IFS0bits.SPI1IF = 0; // Clear the SPI peripheral interrupt flag
    IEC0bits.SPI1IE = 0; // Disable the SPI peripheral interrupt flag
    SPI1CON1bits.DISSCK = 0; // Enable generation of SCK signal
    SPI1CON1bits.DISSDO = 0; // Enable generation of SDO signal
    SPI1CON1bits.MODE16 = 0; // Set 8-bit mode
    SPI1CON1bits.SMP = 0; // Input data sampled at mid-bit
    SPI1CON1bits.CKP = 0; // Idle is SCK low
    SPI1CON1bits.CKE = 1; // Data changes on SCK falling edge
    SPI1BRGL = 39; // Baud rate divisor FCY/100k
    SPI1CON1bits.MSTEN = 1; // Set for master mode
    SPI1CON2 = 0; // Fully disable frame mode    
    
    
    RPINR20bits.SDI1R  = 26; // SPI Data Input (SDI) pin
    TRISGbits.TRISG7 = 1;
    RPOR9bits.RP19R  = 7;    // SPI Data Output (SDO1) pin
    TRISGbits.TRISG8 = 0;
    RPOR10bits.RP21R  = 8;    // SPI Clock (SCK1) pin
    TRISGbits.TRISG6 = 0;
    
    TRISGbits.TRISG9 = 0;
    ANSGbits.ANSG9 = 0;
    LATGbits.LATG9 = 1;
    

    SPI1CON1bits.SPIEN = 1; // enable SPI port, clear status
}


void writeByteSPI(uint8_t dataOut, uint8_t dataIn) 
{
    
    SPI1BUFL = dataOut; // write to buffer for TX
    while(!SPI1STATLbits.SPIRBF); // wait for transfer to complete
    dataIn = SPI1BUFL; // read the received value
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

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
int main ( void )
{
    uint16_t adcResult;
    uint16_t lastAdcResult = 0xFFFF;
    uint8_t dataout = 0xAA;
    uint8_t datain;

    
    /* Call the System Initialize routine*/
    SYS_Initialize ( );
    configureSPI();
    
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
        
        LATGbits.LATG9 = 0;
        writeByteSPI(dataout, datain);
        LATGbits.LATG9 = 1;
       

        
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
                
                printf( "Time %02d:%02d:%02d   Pot = %4d\r\n", 
                        time.hour, 
                        time.minute, 
                        time.second, 
                        adcResult
                      );

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