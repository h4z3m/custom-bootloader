/**
 * @file bl_utils.h
 * @author Hazem Montasser (h4z3m.private@gmail.com)
 * @brief   Utility functions
 * @version 0.1
 * @date 2023-08-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef BL_UTILS_H_
#define BL_UTILS_H_
/*******************************************************************************
 *                              Includes                                       *
 *******************************************************************************/

#include <stdint.h>

/*******************************************************************************
 *                              Definitions                                    *
 *******************************************************************************/

#define VALIDATE_ADDRESS_RANGE(start, end, max_length) \
    (start < end && (end - start + 1) <= max_length)

#define IS_ENUM_MEMBER(enumType, value) \
    (((value) >= (enumType##_FIRST)) && ((value) <= (enumType##_LAST)))

#define BL_VALID_COMMAND(cmd) IS_ENUM_MEMBER(BL_CommandID_t, cmd)

#define CRC32_POLY 0xEDB88320

/*******************************************************************************
 *                            Public functions                                 *
 *******************************************************************************/

uint32_t crc32(const uint8_t *data, uint32_t length);

void erase_flash(uint32_t start_addr, uint32_t length, uint32_t erase_mask);

#endif
