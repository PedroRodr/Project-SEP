/* 
 * File:   i2c.c
 * Authors: pedro, mariana, antonia
 *
 * Created on October 23, 2023
 */

#include <libpic30.h>
#include "i2c.h"
#include "mpu.h"


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