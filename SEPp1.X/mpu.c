/* 
 * File:   mpu.c
 * Authors: pedro, mariana, antonia
 *
 * Created on October 23, 2023
 */

#include <libpic30.h>
#include "mpu.h"
#include "i2c.h"

int Ax,Ay,Az,T,Gx,Gy,Gz;
extern float ax, ay, az, gx, gy, gz, ti;

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
//    configureMPU6050(INT_PIN_CFG, 0x10);        //Configure INT
//    configureMPU6050(INT_ENABLE, 0x01);        //Configure INT
    
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
    ax = (float)Ax/16384.0;
    ay = (float)Ay/16384.0;
    az = (float)Az/16384.0;
    gx = (float)Gx/131.0;
    gy = (float)Gy/131.0;
    gz = (float)Gz/131.0;
    ti = ((float)T/340.00)+36.53;
    
}
