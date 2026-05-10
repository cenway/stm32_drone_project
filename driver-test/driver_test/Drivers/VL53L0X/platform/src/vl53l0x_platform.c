/**
 * @file  vl53l0x_platform.c
 * @brief VL53L0X platform abstraction layer for STM32 HAL
 *
 * Ported from ST's Windows platform layer to STM32 HAL (bare-metal).
 * The original 2-layer structure (platform.c -> i2c_platform.c) is merged
 * into a single file since HAL_I2C_Mem_Read/Write replaces the Windows
 * ranging_sensor_comms DLL entirely.
 *
 * Original: COPYRIGHT (C) STMicroelectronics 2015
 */

#include "vl53l0x_platform.h"
#include "vl53l0x_api.h"

/* ------------------------------------------------------------------ */
/*  Configuration                                                      */
/* ------------------------------------------------------------------ */

/** Maximum I2C transfer size in bytes */
#define VL53L0X_MAX_I2C_XFER_SIZE   64

/** HAL I2C timeout in milliseconds */
#define VL53L0X_I2C_TIMEOUT_MS      100

/* ------------------------------------------------------------------ */
/*  I2C bus serialisation (Lock / Unlock)                               */
/* ------------------------------------------------------------------ */

/**
 * Bare-metal: empty stubs.
 * FreeRTOS:   replace with xSemaphoreTake/Give(i2c_mutex, ...) later.
 */
VL53L0X_Error VL53L0X_LockSequenceAccess(VL53L0X_DEV Dev)
{
    (void)Dev;
    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_UnlockSequenceAccess(VL53L0X_DEV Dev)
{
    (void)Dev;
    return VL53L0X_ERROR_NONE;
}

/* ------------------------------------------------------------------ */
/*  Multi-byte read / write                                            */
/* ------------------------------------------------------------------ */

VL53L0X_Error VL53L0X_WriteMulti(VL53L0X_DEV Dev, uint8_t index,
                                 uint8_t *pdata, uint32_t count)
{
    if (count >= VL53L0X_MAX_I2C_XFER_SIZE) {
        return VL53L0X_ERROR_INVALID_PARAMS;
    }

    HAL_StatusTypeDef hal_status;
    hal_status = HAL_I2C_Mem_Write(Dev->hi2c,
                                   Dev->I2cDevAddr,
                                   index,
                                   I2C_MEMADD_SIZE_8BIT,
                                   pdata,
                                   (uint16_t)count,
                                   VL53L0X_I2C_TIMEOUT_MS);

    if (hal_status != HAL_OK) {
        return VL53L0X_ERROR_CONTROL_INTERFACE;
    }

    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_ReadMulti(VL53L0X_DEV Dev, uint8_t index,
                                uint8_t *pdata, uint32_t count)
{
    if (count >= VL53L0X_MAX_I2C_XFER_SIZE) {
        return VL53L0X_ERROR_INVALID_PARAMS;
    }

    HAL_StatusTypeDef hal_status;
    hal_status = HAL_I2C_Mem_Read(Dev->hi2c,
                                  Dev->I2cDevAddr,
                                  index,
                                  I2C_MEMADD_SIZE_8BIT,
                                  pdata,
                                  (uint16_t)count,
                                  VL53L0X_I2C_TIMEOUT_MS);

    if (hal_status != HAL_OK) {
        return VL53L0X_ERROR_CONTROL_INTERFACE;
    }

    return VL53L0X_ERROR_NONE;
}

/* ------------------------------------------------------------------ */
/*  Single byte / word / dword write                                   */
/* ------------------------------------------------------------------ */

VL53L0X_Error VL53L0X_WrByte(VL53L0X_DEV Dev, uint8_t index, uint8_t data)
{
    return VL53L0X_WriteMulti(Dev, index, &data, 1);
}

VL53L0X_Error VL53L0X_WrWord(VL53L0X_DEV Dev, uint8_t index, uint16_t data)
{
    uint8_t buffer[2];

    /* VL53L0X is big-endian: MSB first */
    buffer[0] = (uint8_t)(data >> 8);
    buffer[1] = (uint8_t)(data & 0x00FF);

    return VL53L0X_WriteMulti(Dev, index, buffer, 2);
}

VL53L0X_Error VL53L0X_WrDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t data)
{
    uint8_t buffer[4];

    /* VL53L0X is big-endian: MSB first */
    buffer[0] = (uint8_t)(data >> 24);
    buffer[1] = (uint8_t)((data >> 16) & 0xFF);
    buffer[2] = (uint8_t)((data >>  8) & 0xFF);
    buffer[3] = (uint8_t)(data & 0xFF);

    return VL53L0X_WriteMulti(Dev, index, buffer, 4);
}

/* ------------------------------------------------------------------ */
/*  Single byte / word / dword read                                    */
/* ------------------------------------------------------------------ */

VL53L0X_Error VL53L0X_RdByte(VL53L0X_DEV Dev, uint8_t index, uint8_t *data)
{
    return VL53L0X_ReadMulti(Dev, index, data, 1);
}

VL53L0X_Error VL53L0X_RdWord(VL53L0X_DEV Dev, uint8_t index, uint16_t *data)
{
    VL53L0X_Error status;
    uint8_t buffer[2];

    status = VL53L0X_ReadMulti(Dev, index, buffer, 2);
    if (status == VL53L0X_ERROR_NONE) {
        *data = ((uint16_t)buffer[0] << 8) | (uint16_t)buffer[1];
    }

    return status;
}

VL53L0X_Error VL53L0X_RdDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t *data)
{
    VL53L0X_Error status;
    uint8_t buffer[4];

    status = VL53L0X_ReadMulti(Dev, index, buffer, 4);
    if (status == VL53L0X_ERROR_NONE) {
        *data = ((uint32_t)buffer[0] << 24)
              | ((uint32_t)buffer[1] << 16)
              | ((uint32_t)buffer[2] <<  8)
              |  (uint32_t)buffer[3];
    }

    return status;
}

/* ------------------------------------------------------------------ */
/*  Read-modify-write                                                  */
/* ------------------------------------------------------------------ */

VL53L0X_Error VL53L0X_UpdateByte(VL53L0X_DEV Dev, uint8_t index,
                                 uint8_t AndData, uint8_t OrData)
{
    VL53L0X_Error status;
    uint8_t data;

    status = VL53L0X_RdByte(Dev, index, &data);

    if (status == VL53L0X_ERROR_NONE) {
        data = (data & AndData) | OrData;
        status = VL53L0X_WrByte(Dev, index, data);
    }

    return status;
}

/* ------------------------------------------------------------------ */
/*  Polling delay                                                      */
/* ------------------------------------------------------------------ */

VL53L0X_Error VL53L0X_PollingDelay(VL53L0X_DEV Dev)
{
    (void)Dev;

    /* Bare-metal: HAL_Delay(1) gives ~1ms between polls.
     * FreeRTOS:   replace with vTaskDelay(pdMS_TO_TICKS(1)) later.
     */
    HAL_Delay(5);

    return VL53L0X_ERROR_NONE;
}
