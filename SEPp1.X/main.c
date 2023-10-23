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


#define TRUE 1
#define FALSE 0

#define I2C_READ 1
#define I2C_WRITE 0

#define PRIORITY 3

// - - - - - - - - - - - - - - - MPU6050 - - - - - - - - - - - - - - - - - -
#define SMPLRT_DIV 0x19
#define CONFIG 0x1A
#define GYRO_CONFIG  0x1B
#define ACCEL_CONFIG 0x1C
#define INT_PIN_CFG 0x37
#define INT_ENABLE 0x38
#define DMP_INT_STATUS 0x39
#define INT_STATUS 0x3A
#define ACCEL_XOUT 0x3B
#define ACCEL_YOUT 0x3D
#define ACCEL_ZOUT 0x3F
#define GYRO_XOUT 0x43
#define GYRO_YOUT 0x45
#define GYRO_ZOUT 0x47
#define MPU6050_ADD 0x68
#define PWR_MGMT_1 0x6B
#define REGISTER_ADD 0x75
#define MPU6050_ADD_W 0xD0
#define MPU6050_ADD_R 0xD1


#define MPU6050_INT_LAT LATBbits.LATB15


// Define the SPI pins
#define SPI_SDI_PIN  RPINR20bits.SDI1R   // SPI Data Input (SDI) pin
#define SPI_SDO_PIN  RPOR3bits.RP7R      // SPI Data Output (SDO1) pin
#define SPI_SCK_PIN  RPOR4bits.RP8R      // SPI Clock (SCK1) pin

// - - - - - - - - - - - Global variables - - - - - - - - - - - - - - - - - -
int Ax,Ay,Az,T,Gx,Gy,Gz;
float AX, AY, AZ, t, GX, GY, GZ;
volatile uint16_t iocFlag = 0; // Shared flag, volatile attribute prevents dangerous optimization

// - - - - - - - - - -  - - - - - - - I2C - - - - - - - - - - - - - - - - - -
void I2C_ACK(void) {
	I2C1CONLbits.ACKDT = 0;			// 0 -> ACK
    I2C1CONLbits.ACKEN = 1;			// Send ACK
    while(I2C1CONLbits.ACKEN);
}

void I2C_NACK(void){
	I2C1CONLbits.ACKDT = 1;			// 1 -> NACK
	I2C1CONLbits.ACKEN = 1;			// Send NACK
    while(I2C1CONLbits.ACKEN);
}


void initializeI2C() {
    
    I2C1CONLbits.I2CEN = 0; // Disable I2C peripheral during setup
    I2C1CONLbits.A10M = 0; // 7 bit address mode
    I2C1BRG = 0x4E; // 100KHz for a 4:1 (with FCY = 16 MHz)
    
    ANSBbits.ANSB9 = 0;
    TRISBbits.TRISB8 = 1;
    TRISBbits.TRISB9 = 1;
    
    I2C1CONLbits.I2CEN = 1; //enable I2C1
    
}

void startConditionI2C(){
    
    I2C1CONLbits.SEN = 1; // Initiate the start condition on SDA and SCL pins
    while(I2C1CONLbits.SEN ); // Wait till sequence is completed
    
}

void restartConditionI2C(){
    
    I2C1CONLbits.RSEN = 1; // Initiate the start condition on SDA and SCL pins
    while(I2C1CONLbits.RSEN ); // Wait till sequence is completed 
    
}

void stopConditionI2C() {
    
    I2C1CONLbits.PEN = 1; // Initiate the stop condition on SDA and SCL pins
    while(I2C1CONLbits.PEN); // Wait till sequence is completed
    
}


uint8_t writeByteI2C(uint8_t data) {
    
    I2C1TRN = data; // Move data to the I2CxTRN register, transmission starts right after
    while(I2C1STATbits.TRSTAT == 1);
    if(!I2C1STATbits.ACKSTAT) return 0; // Check for the slave acknowledge / no error code
    return -1; // Error code
    
}


uint8_t readByteI2C(unsigned char ACK_NACK) {
        
    I2C1CONLbits.RCEN = 1; // Assert the receive enable bit
    while(!I2C1STATbits.RBF);
    if(ACK_NACK==0) I2C_ACK();            
    else I2C_NACK(); 
    return I2C1RCV; // Check if the receive buffer is full / return the received data
    
}

//unsigned char I2C_Read(unsigned char ACK_NACK)
//{  
//    //---[ Receive & Return A Byte & Send ACK or NACK ]---
//    unsigned char Data;
//    RCEN = 1;              
//    while(!BF);      
//    Data = SSPBUF;           
//    if(ACK_NACK==0)
//        I2C_ACK();            
//    else
//        I2C_NACK();     
//    while(!SSPIF);                 
//    SSPIF=0;   
//    return Data;
//}


void readRegisterData(uint8_t registerRead){
    
    startConditionI2C();
    // Send the device address with write operation
    writeByteI2C(MPU6050_ADD_W);
    // Send the register address to read from
    writeByteI2C(registerRead);    
    startConditionI2C();
    writeByteI2C(MPU6050_ADD_R);    
    readByteI2C(registerRead);    
    stopConditionI2C();
    
}


// - - - - - - - - - - - - - - - Interrupts - - - - - - - - - - - - - - - --

void initializeInterruption()
{
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


// - - - - - - - - - - - - - - - MPU6050- - - - - - - - - - - - - - - - - -

void configureMPU6050(uint8_t address, uint8_t option){
    
    //  - - - GYRO - - -
    // 0x00 - +/- 250 o/sec
    // 0x01 - +/- 500 o/sec
    // 0x10 - +/- 1000 o/sec
    // 0x18 - +/- 2000 o/sec
    
    //  - - - ACCEL - - -
    // 0x00 - +/- 2g
    // 0x01 - +/- 4g
    // 0x10 - +/- 8g
    // 0x18 - +/- 16g
    
    startConditionI2C();
    writeByteI2C(MPU6050_ADD_W);
    writeByteI2C(address);    
    writeByteI2C(option);     
    stopConditionI2C();
    
}

void initializeMPU6050(){
    
    configureMPU6050(GYRO_CONFIG, 0x18);        //Configure Gyroscope
    configureMPU6050(ACCEL_CONFIG, 0x00);       //Configure Accelerometer
    configureMPU6050(SMPLRT_DIV, 0x07);         //Configure Sample (Data) Rate
    configureMPU6050(PWR_MGMT_1, 0x01);         //Configure Clock Source
    configureMPU6050(CONFIG, 0x00);             //Configure DLPF
    configureMPU6050(INT_PIN_CFG, 0x10);        //Configure INT
    configureMPU6050(INT_ENABLE, 0x01);        //Configure INT
    
//    TRISBbits.TRISB6 = 0; // configure MPU6050 INT pin as output
//    LATBbits.LATB6 = 0; // configure MPU6050 INT pin to a known state (low)
    
}

void MPU6050(){
        
    startConditionI2C(); // Start I2C communication
    
    // Send the device address with write operation
    writeByteI2C(MPU6050_ADD_W);
    
    // Send the register address to read from
    writeByteI2C(0x3B);
    
    startConditionI2C();
    
    writeByteI2C(MPU6050_ADD_R);
       
    //readByteI2C(0);  
    
    Ax = (((uint16_t) readByteI2C(0))<<8) | (uint16_t) readByteI2C(0);
    Ay = (((uint16_t) readByteI2C(0))<<8) | (uint16_t) readByteI2C(0);
    Az = (((uint16_t) readByteI2C(0))<<8) | (uint16_t) readByteI2C(0);
    T  = (((uint16_t) readByteI2C(0))<<8) | (uint16_t) readByteI2C(0);  
    Gx = (((uint16_t) readByteI2C(0))<<8) | (uint16_t) readByteI2C(0);
    Gy = (((uint16_t) readByteI2C(0))<<8) | (uint16_t) readByteI2C(0);
    Gz = (((uint16_t) readByteI2C(0))<<8) | (uint16_t) readByteI2C(1);
    
//    // - - - ACK - - -
//    I2C1CONLbits.ACKDT = 0;
//    I2C1CONLbits.ACKEN = 1;
//    while(I2C1CONLbits.ACKEN);
//    // - - - - - - - - 
    
    stopConditionI2C();
    
    // Convert The Readings
    AX = (float)Ax/16384.0;
    AY = (float)Ay/16384.0;
    AZ = (float)Az/16384.0;
    GX = (float)Gx/131.0;
    GY = (float)Gy/131.0;
    GZ = (float)Gz/131.0;
    t = ((float)T/340.00)+36.53;
    
}


// - - - - - - - - - - - - - - - SPI- - - - - - - - - - - - - - - - -  - - -

void configureSPISlave()
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


void writeByteSPI(uint8_t dataOut, uint8_t dataIn) 
{   
    SPI1BUFL = dataOut; // write to buffer for TX
    while(!SPI1STATLbits.SPIRBF); // wait for transfer to complete
    dataIn = SPI1BUFL; // read the received value
}


int main(int argc, char** argv) 
{

    uint8_t dataout = 0xAA;
    uint8_t datain;
    
    initializeI2C();
    initializeMPU6050();
    configureSPISlave();
    
//    ANSAbits.ANSA0 = 0; // Unset the pin ANS bit, make sure analog functionality is disabled
//    LATAbits.LATA0 = 1; // Set the pin LAT bit, set outputs to 1
//    TRISAbits.TRISA0 = 0; // Unset the pin TRIS bit, enables the digital output
//    
//    
//    ANSBbits.ANSB1 = 0; // Unset the pin ANS bit, make sure analog functionality is disabled
//    LATBbits.LATB1 = 1; // Set the pin LAT bit, set outputs to 1
//    TRISBbits.TRISB1 = 0; // Unset the pin TRIS bit, enables the digital output
    
    while(1) {
        writeByteSPI(dataout, datain);
        //if(iocFlag)
        //{ 
            //MPU6050();
            //iocFlag = 0;
        //}
        //MPU6050(); 

        // Perform your MPU6050 data reading here
        MPU6050();  // Reads data from MPU6050

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