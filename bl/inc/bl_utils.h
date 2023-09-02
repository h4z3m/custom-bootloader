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

#define VALIDATE_CMD(data, length, crc) \
	(bl_calculate_command_crc(data, length) == crc)

/*******************************************************************************
 *                            Public functions                                 *
 *******************************************************************************/

/**
 * @fn uint32_t bl_calculate_command_crc(void*, uint32_t)
 * @brief	Calculates the CRC for a command
 *
 * @param command	Pointer to the command struct
 * @param size		Size of the command in bytes
 * @return
 */
uint32_t bl_calculate_command_crc(void *command, uint32_t size);

#endif
