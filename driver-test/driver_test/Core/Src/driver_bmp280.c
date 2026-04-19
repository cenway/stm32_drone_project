/*
 * driver_bmp280.c
 *
 *  Created on: 2026. 4. 17.
 *      Author: yhj02
 */

#include "driver_bmp280.h"
#include <math.h>

#define BMP280_ADDR (0x76 << 1)
#define BMP280_ID 0xD0
#define BMP280_DATA 0xF7

static float altitude;
static float press_gnd;
static float temp;
static float press;
static uint32_t temp_raw;
static uint32_t press_raw;
static uint8_t bmp280_buf[6];
static bmp280_calib_t bmp280_calib;

static int32_t t_fine;

const float * bmp280_get_temp(void)
{
	return &temp;
}

const float * bmp280_get_altitude(void)
{
	return &altitude;
}


const float * bmp280_get_press(void)
{
	return &press;
}


void bmp280_write(uint16_t memaddr, uint8_t * write_data, uint16_t size)
{
	HAL_I2C_Mem_Write(&hi2c1, BMP280_ADDR, memaddr, 1, write_data, size, 100);
}

void bmp280_read(uint16_t memaddr, uint8_t * read_data, uint16_t size)
{
	HAL_I2C_Mem_Read(&hi2c1, BMP280_ADDR, memaddr, 1, read_data, size, 100);
}

void bmp280_read_IT(uint16_t memaddr, uint8_t * read_data, uint16_t size)
{
	HAL_I2C_Mem_Read_IT(&hi2c1, BMP280_ADDR, memaddr, 1, read_data, size);
}

void bmp280_read_data(void)
{
    bmp280_read(BMP280_DATA, bmp280_buf, 6);
}

void bmp280_read_data_IT(void)
{
    bmp280_read_IT(BMP280_DATA, bmp280_buf, 6);
}

void bmp280_parse(void)
{
	press_raw = ((uint32_t)bmp280_buf[0] << 12) |
				((uint32_t)bmp280_buf[1] << 4)  |
				((uint32_t)bmp280_buf[2] >> 4);

	temp_raw  = ((uint32_t)bmp280_buf[3] << 12) |
				((uint32_t)bmp280_buf[4] << 4)  |
				((uint32_t)bmp280_buf[5] >> 4);
}

/**
 * BMP280 Compensation Formulas
 * Source: BMP280 Datasheet (BST-BMP280-DS001-26) Section 3.11.3
 * 64-bit integer version for STM32F4
 *
 * Usage:
 *   1. Read calibration data (0x88~0xA1) at init and populate dig_T1~T3, dig_P1~P9
 *   2. Every measurement: call compensate_T first, then compensate_P
 *   3. Temperature output: value / 100.0 = degrees C  (e.g. 5123 = 51.23 C)
 *   4. Pressure output:    value / 256.0 = Pa          (e.g. 24674867 = 96386.2 Pa)
 */


/* Global: fine temperature value, shared between T and P compensation */


/* ---------- Temperature Compensation ----------
 * Returns temperature in DegC, resolution is 0.01 DegC.
 * Output value of "5123" equals 51.23 DegC.
 */
void bmp280_compensate_T_int32(int32_t adc_T, const bmp280_calib_t *calib)
{
    int32_t var1, var2, T;

    var1 = ((((adc_T >> 3) - ((int32_t)calib->dig_T1 << 1))) *
            ((int32_t)calib->dig_T2)) >> 11;

    var2 = (((((adc_T >> 4) - ((int32_t)calib->dig_T1)) *
              ((adc_T >> 4) - ((int32_t)calib->dig_T1))) >> 12) *
            ((int32_t)calib->dig_T3)) >> 14;

    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;

    temp = (float)T/100.0f;
}

/* ---------- Pressure Compensation (64-bit) ----------
 * Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format
 * (24 integer bits and 8 fractional bits).
 * Output value of "24674867" represents 24674867 / 256 = 96386.2 Pa = 963.862 hPa
 */
void bmp280_compensate_P_int64(int32_t adc_P, const bmp280_calib_t *calib)
{
    int64_t var1, var2, p;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)calib->dig_P6;
    var2 = var2 + ((var1 * (int64_t)calib->dig_P5) << 17);
    var2 = var2 + (((int64_t)calib->dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)calib->dig_P3) >> 8) +
           ((var1 * (int64_t)calib->dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)calib->dig_P1) >> 33;

    if (var1 == 0)
    {
        return;  /* avoid exception caused by division by zero */
    }

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)calib->dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)calib->dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)calib->dig_P7) << 4);

    press = (float)p/256.0f;
}

//call after init
void bmp280_ground(int samples)
{
	press_gnd = 0.0f;

	// 첫 20샘플은 버림 (안전장치)
	for(int i = 0; i < 20; i++) {
		bmp280_read_data();
		bmp280_update();
		HAL_Delay(40);
	}


	for(int i = 0; i < samples; i++)
	{
		bmp280_read_data();
		bmp280_update();
		press_gnd += press;
		HAL_Delay(40);
	}
	press_gnd /= samples;
}

void bmp280_update(void)
{
	bmp280_parse();
	bmp280_compensate_T_int32(temp_raw, &bmp280_calib);
	bmp280_compensate_P_int64(press_raw, &bmp280_calib);
	if(press_gnd != 0.0f)
		altitude = 44330.0f * (1.0f - powf(press / press_gnd, 0.1903f));
	else
		altitude = -1.0f;
}


void bmp280_init(void)
{
	uint8_t data;

	//reset
	data = 0xB6;
	bmp280_write(0xE0, &data, 1);
	HAL_Delay(100);

	bmp280_read(0x88, (uint8_t*)&bmp280_calib, 24);
	HAL_Delay(10);

	//config : Standby(0.5ms), IIR(x16), spi3w(off)
	data = 0x10; //0b000_100_00
	bmp280_write(0xF5, &data, 1);
	HAL_Delay(10);

	//ctrl : Oversampling_t(x2), Oversampling_t(x16), mode(normal)
	data = 0x57; //0b010_101_11
	bmp280_write(0xF4, &data, 1);

	HAL_Delay(1000);
}
