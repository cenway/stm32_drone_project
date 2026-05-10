/*
 * driver_vl53l0x.c
 *
 *  Created on: 2026. 5. 10.
 *      Author: yhj02
 */

#include "driver_vl53l0x.h"
#include <stdio.h>

#define VL53L0X_CALIB_SPAD_COUNT      5
#define VL53L0X_CALIB_IS_APERTURE     0
#define VL53L0X_CALIB_VHV_SETTINGS    31
#define VL53L0X_CALIB_PHASE_CAL       0
#define VL53L0X_CALIB_OFFSET_UM       35000


static VL53L0X_Dev_t    vl53l0x_dev;
static VL53L0X_DEV      Dev = &vl53l0x_dev;

VL53L0X_Error vl53l0x_init(void)
{
	 vl53l0x_dev.I2cDevAddr      = 0x52;
	 vl53l0x_dev.comms_type      = 1;     /* I2C */
	 vl53l0x_dev.comms_speed_khz = 400;
	 vl53l0x_dev.hi2c            = &hi2c1;

	 VL53L0X_Error 	status;
	 char buf[100];

	 status = VL53L0X_DataInit(Dev);
	 sprintf(buf, "status: %d\r\n", status);
	 HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 100);
	 if (status != VL53L0X_ERROR_NONE) return status;


	 status = VL53L0X_StaticInit(Dev);
	 sprintf(buf, "status: %d\r\n", status);
	 HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 100);
	 if (status != VL53L0X_ERROR_NONE) return status;

	 status = VL53L0X_SetReferenceSpads(Dev, VL53L0X_CALIB_SPAD_COUNT,
			 	 VL53L0X_CALIB_IS_APERTURE);
	 sprintf(buf, "status: %d\r\n", status);
	 HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 100);
	 if (status != VL53L0X_ERROR_NONE) return status;

	 status = VL53L0X_SetRefCalibration(Dev, VL53L0X_CALIB_VHV_SETTINGS,
			 	 VL53L0X_CALIB_PHASE_CAL);
	 sprintf(buf, "status: %d\r\n", status);
	 HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 100);
	 if (status != VL53L0X_ERROR_NONE) return status;

	 status = VL53L0X_SetOffsetCalibrationDataMicroMeter(Dev, VL53L0X_CALIB_OFFSET_UM);
	 sprintf(buf, "status: %d\r\n", status);
	 HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 100);
	 if (status != VL53L0X_ERROR_NONE) return status;

	 status = VL53L0X_SetLimitCheckValue(Dev,
	                  VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,
	                  (FixPoint1616_t)(0.25 * 65536));
	 sprintf(buf, "status: %d\r\n", status);
	 HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 100);
	 if (status != VL53L0X_ERROR_NONE) return status;

	 status = VL53L0X_SetLimitCheckValue(Dev,
	                  VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,
	                  (FixPoint1616_t)(32 * 65536));
	 sprintf(buf, "status: %d\r\n", status);
	 HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 100);
	 if (status != VL53L0X_ERROR_NONE) return status;

	 status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(Dev, 20000);
	 sprintf(buf, "status: %d\r\n", status);
	 HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 100);
	 if (status != VL53L0X_ERROR_NONE) return status;

	 status = VL53L0X_SetDeviceMode(Dev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
	 if (status != VL53L0X_ERROR_NONE) return status;

	 return status;
}

VL53L0X_Error vl53l0x_start(void)
{
	char buf[100];
	VL53L0X_Error 	status;
	status = VL53L0X_StartMeasurement(Dev);
	sprintf(buf, "status: %d\r\n", status);
	HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 100);
	return status;
}

void vl53l0x_ready(uint8_t *pMeasurementDataReady)
{
	VL53L0X_GetMeasurementDataReady(Dev, pMeasurementDataReady);
}

void vl53l0x_range(VL53L0X_RangingMeasurementData_t *pRangingMeasurementData)
{
	VL53L0X_GetRangingMeasurementData(Dev, pRangingMeasurementData);
	VL53L0X_ClearInterruptMask(Dev, 0);
}
