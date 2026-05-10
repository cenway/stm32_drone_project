/**
 * @file  vl53l0x_platform_log.h
 * @brief VL53L0X platform log function definition
 *
 * Logging is disabled by default (VL53L0X_LOG_ENABLE not defined).
 * All log macros expand to empty statements to save flash.
 *
 * Original: COPYRIGHT (C) STMicroelectronics 2015
 */

#ifndef _VL53L0X_PLATFORM_LOG_H_
#define _VL53L0X_PLATFORM_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Log levels */
enum {
    TRACE_LEVEL_NONE,
    TRACE_LEVEL_ERRORS,
    TRACE_LEVEL_WARNING,
    TRACE_LEVEL_INFO,
    TRACE_LEVEL_DEBUG,
    TRACE_LEVEL_ALL,
    TRACE_LEVEL_IGNORE
};

/* Log functions */
enum {
    TRACE_FUNCTION_NONE = 0,
    TRACE_FUNCTION_I2C  = 1,
    TRACE_FUNCTION_ALL  = 0x7fffffff
};

/* Log modules */
enum {
    TRACE_MODULE_NONE     = 0x0,
    TRACE_MODULE_API      = 0x1,
    TRACE_MODULE_PLATFORM = 0x2,
    TRACE_MODULE_ALL      = 0x7fffffff
};

/* Logging disabled: all macros are no-ops */
#define VL53L0X_ErrLog(...)                         (void)0
#define _LOG_FUNCTION_START(module, fmt, ...)        (void)0
#define _LOG_FUNCTION_END(module, status, ...)       (void)0
#define _LOG_FUNCTION_END_FMT(module, status, fmt, ...) (void)0

/* String copy macro used by API */
#include <string.h>
#define VL53L0X_COPYSTRING(str, ...) strcpy(str, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* _VL53L0X_PLATFORM_LOG_H_ */
