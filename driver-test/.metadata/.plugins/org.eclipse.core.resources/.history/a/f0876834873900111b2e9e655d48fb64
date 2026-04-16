/*
 * driver_mpu6500.h
 *
 *  Created on: 2026. 4. 12.
 *      Author: yhj02
 */

#ifndef INC_DRIVER_MPU6500_H_
#define INC_DRIVER_MPU6500_H_

#include "stm32f4xx_hal.h"

#define MPU6500_ADDR (0x68 << 1)
#define MPU6500_WHOAMI 0x75

extern I2C_HandleTypeDef hi2c1;

typedef struct
{
	int16_t x;
	int16_t y;
	int16_t z;
}mpu6500_t;

typedef struct
{
	float x;
	float y;
	float z;
}mpu6500_conv_t;

typedef struct
{
    float roll;
    float pitch;
    float yaw;
} mpu6500_euler_t;

typedef struct
{
    mpu6500_t accel;
    mpu6500_t gyro;
    mpu6500_conv_t acc_cnv;
    mpu6500_conv_t gyr_cnv;
    mpu6500_conv_t gyr_offset;
    mpu6500_euler_t euler;
} mpu6500_data_t;


void mpu6500_write(uint16_t memaddr, uint8_t * write_data, uint16_t size);
void mpu6500_read(uint16_t memaddr, uint8_t * read_data, uint16_t size);
void mpu6500_read_IT(uint16_t memaddr, uint8_t * read_data, uint16_t size);
void mpu6500_read_data(mpu6500_t *accel, mpu6500_t *gyro);
void mpu6500_read_data_IT(uint8_t * buf);
void mpu6500_conv_data(mpu6500_t *accel, mpu6500_t *gyro,
		mpu6500_conv_t *acc_cnv, mpu6500_conv_t *gyr_cnv, mpu6500_conv_t *gyr_offset);
void mpu6500_complementary_filter(mpu6500_conv_t *accel, mpu6500_conv_t *gyro,
                                   mpu6500_euler_t *euler, float dt);
void mpu6500_update(void);
void mpu6500_calibrate(mpu6500_conv_t *gyro_offset, int samples);
void mpu6500_init(void);

#endif /* INC_DRIVER_MPU6500_H_ */
