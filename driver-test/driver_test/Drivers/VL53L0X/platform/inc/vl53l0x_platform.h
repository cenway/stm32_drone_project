/**
 * @file  vl53l0x_platform.h
 * @brief VL53L0X platform abstraction layer for STM32 HAL
 *
 * Ported from ST's Windows platform layer to STM32 HAL (bare-metal).
 * Original: COPYRIGHT (C) STMicroelectronics 2015
 */

#ifndef _VL53L0X_PLATFORM_H_
#define _VL53L0X_PLATFORM_H_

#include "vl53l0x_def.h"
#include "vl53l0x_platform_log.h"
#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup VL53L0X_platform_group VL53L0X Platform Functions
 * @brief    VL53L0X Platform Functions
 *  @{
 */

/**
 * @struct  VL53L0X_Dev_t
 * @brief   Generic PAL device type that does link between API and platform
 *          abstraction layer
 */
typedef struct {
    VL53L0X_DevData_t   Data;               /*!< embed ST Ewok Dev data as "Data"   */

    /*!< user specific fields */
    uint8_t             I2cDevAddr;          /*!< i2c device address (8-bit, e.g. 0x52) */
    uint8_t             comms_type;          /*!< VL53L0X_COMMS_I2C or VL53L0X_COMMS_SPI */
    uint16_t            comms_speed_khz;     /*!< Comms speed [kHz] : typically 400  */

    /*!< STM32 HAL specific */
    I2C_HandleTypeDef  *hi2c;               /*!< pointer to HAL I2C handle          */
} VL53L0X_Dev_t;

/**
 * @brief   Declare the device Handle as a pointer of the structure
 *          @a VL53L0X_Dev_t.
 */
typedef VL53L0X_Dev_t *VL53L0X_DEV;

/**
 * @def PALDevDataGet
 * @brief Get ST private structure @a VL53L0X_DevData_t data access
 */
#define PALDevDataGet(Dev, field)        (Dev->Data.field)

/**
 * @def PALDevDataSet
 * @brief Set ST private structure @a VL53L0X_DevData_t data field
 */
#define PALDevDataSet(Dev, field, data)  (Dev->Data.field) = (data)

/** @defgroup VL53L0X_registerAccess_group PAL Register Access Functions
 *  @brief    PAL Register Access Functions
 *  @{
 */

VL53L0X_Error VL53L0X_LockSequenceAccess(VL53L0X_DEV Dev);
VL53L0X_Error VL53L0X_UnlockSequenceAccess(VL53L0X_DEV Dev);

VL53L0X_Error VL53L0X_WriteMulti(VL53L0X_DEV Dev, uint8_t index,
                                 uint8_t *pdata, uint32_t count);
VL53L0X_Error VL53L0X_ReadMulti(VL53L0X_DEV Dev, uint8_t index,
                                uint8_t *pdata, uint32_t count);

VL53L0X_Error VL53L0X_WrByte(VL53L0X_DEV Dev, uint8_t index, uint8_t data);
VL53L0X_Error VL53L0X_WrWord(VL53L0X_DEV Dev, uint8_t index, uint16_t data);
VL53L0X_Error VL53L0X_WrDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t data);

VL53L0X_Error VL53L0X_RdByte(VL53L0X_DEV Dev, uint8_t index, uint8_t *data);
VL53L0X_Error VL53L0X_RdWord(VL53L0X_DEV Dev, uint8_t index, uint16_t *data);
VL53L0X_Error VL53L0X_RdDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t *data);

VL53L0X_Error VL53L0X_UpdateByte(VL53L0X_DEV Dev, uint8_t index,
                                 uint8_t AndData, uint8_t OrData);

/** @} end of VL53L0X_registerAccess_group */

/**
 * @brief execute delay in all polling API call
 *
 * Bare-metal: uses HAL_Delay().
 * FreeRTOS:   replace with vTaskDelay(pdMS_TO_TICKS(1)) later.
 */
VL53L0X_Error VL53L0X_PollingDelay(VL53L0X_DEV Dev);

/** @} end of VL53L0X_platform_group */

#ifdef __cplusplus
}
#endif

#endif /* _VL53L0X_PLATFORM_H_ */
