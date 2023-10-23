/* 
 * File:   mpu.h
 * Authors: pedro, mariana, antonia
 *
 * Created on October 23, 2023
 */

#ifndef MPU_H
#define MPU_H

#include <stdint.h>
#include <p24FJ256GA702.h>
#include <libpic30.h>

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

void configureMPU6050(uint8_t address, uint8_t option);
void initializeMPU6050();
void MPU6050();

#endif // MPU_H
