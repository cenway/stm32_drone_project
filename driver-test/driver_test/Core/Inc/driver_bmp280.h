/*
 * driver_bmp280.h
 *
 *  Created on: 2026. 4. 17.
 *      Author: yhj02
 */

#ifndef INC_DRIVER_BMP280_H_
#define INC_DRIVER_BMP280_H_

#include "stm32f4xx_hal.h"

extern I2C_HandleTypeDef hi2c1;

/* ---------- Calibration data (read from NVM at init) ---------- */
typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
} bmp280_calib_t;


const float * bmp280_get_press(void);
void bmp280_write(uint16_t memaddr, uint8_t * write_data, uint16_t size);
void bmp280_read(uint16_t memaddr, uint8_t * read_data, uint16_t size);
void bmp280_read_IT(uint16_t memaddr, uint8_t * read_data, uint16_t size);
void bmp280_read_data_IT(void);
void bmp280_parse(void);
void bmp280_compensate_T_int32(int32_t adc_T, const bmp280_calib_t *calib);
void bmp280_compensate_P_int64(int32_t adc_P, const bmp280_calib_t *calib);
void bmp280_update(void);
void bmp280_init(void);
#endif /* INC_DRIVER_BMP280_H_ */
