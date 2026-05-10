/**
 * @file  vl53l0x_types.h
 * @brief VL53L0X types definition
 *
 * STM32 toolchain provides stdint.h and stddef.h, so no porting needed.
 *
 * Original: COPYRIGHT (C) STMicroelectronics 2015
 */

#ifndef VL53L0X_TYPES_H_
#define VL53L0X_TYPES_H_

/** @defgroup porting_type  Basic type definition
 *  @ingroup  VL53L0X_platform_group
 *
 *  @brief  file vl53l0x_types.h files hold basic type definition that may
 *          requires porting
 *
 *  contains type that must be defined for the platform\n
 *  when target platform and compiler provide stdint.h and stddef.h it is
 *  enough to include it.\n
 */
#include <stdint.h>
#include <stddef.h>

#ifndef NULL
#error "Error NULL definition should be done. Please add required include"
#endif

/** use where fractional values are expected
 *
 * Given a floating point value f it's .16 bit point is (int)(f*(1<<16)) */
typedef uint32_t FixPoint1616_t;

#endif /* VL53L0X_TYPES_H_ */
