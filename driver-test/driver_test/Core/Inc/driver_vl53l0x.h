/*
 * driver_vl53l0x.h
 *
 *  Created on: 2026. 5. 10.
 *      Author: yhj02
 */

#ifndef SRC_DRIVER_VL53L0X_H_
#define SRC_DRIVER_VL53L0X_H_

#include "vl53l0x_api.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;
VL53L0X_Error vl53l0x_init(void);
VL53L0X_Error vl53l0x_start(void);
void vl53l0x_ready(uint8_t *pMeasurementDataReady);
void vl53l0x_range(VL53L0X_RangingMeasurementData_t *pRangingMeasurementData);


#endif /* SRC_DRIVER_VL53L0X_H_ */
