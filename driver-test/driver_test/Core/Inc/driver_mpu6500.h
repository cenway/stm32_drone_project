/*
 * driver_mpu6500.h
 *
 *  Created on: 2026. 4. 12.
 *      Author: yhj02
 */

//#include "driver_mpu6500.h"
//#define CALIB_MPU6500 1000
//
//mpu6500_init();
//mpu6500_calibrate(CALIB_MPU6500);
//if(time_flag_1ms)
//{
//  if(hi2c1.State == HAL_I2C_STATE_READY)
//	  mpu6500_read_data_IT();
//  time_flag_1ms = 0;
//}
//
//if(mpu6500_data_received)
//{
//  mpu6500_parse();
//  mpu6500_update();
//  mpu6500_data_received = 0;
//}
//
//if(time_flag_100ms)
//{
//  const mpu6500_euler_t *euler = mpu6500_get_euler();
//  int r = (int)(euler->roll*100);
//  int p = (int)(euler->pitch*100);
//  int y = (int)(euler->yaw*100);
//
//  char buf[80];
//  sprintf(buf, "%d.%.2d ,%d.%.2d ,%d.%.2d\r\n",
//		  r/100, abs(r%100), p/100, abs(p%100), y/100, abs(y%100));
//  HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 100);
//
//  time_flag_100ms = 0;
//}

#ifndef INC_DRIVER_MPU6500_H_
#define INC_DRIVER_MPU6500_H_

#include "stm32f4xx_hal.h"

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

const mpu6500_euler_t *mpu6500_get_euler(void);

void mpu6500_write(uint16_t memaddr, uint8_t * write_data, uint16_t size);
void mpu6500_read(uint16_t memaddr, uint8_t * read_data, uint16_t size);
void mpu6500_read_IT(uint16_t memaddr, uint8_t * read_data, uint16_t size);
void mpu6500_parse(void);
void mpu6500_read_data(void);
void mpu6500_read_data_IT(void);
void mpu6500_conv_data(void);
void mpu6500_calibrate(int samples);
void mpu6500_complementary_filter(float dt);
void mpu6500_update(void);

void mpu6500_init(void);

#endif /* INC_DRIVER_MPU6500_H_ */
