/**
 * @file bl_utils.c
 * @author Hazem Montasser (h4z3m.private@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-14
 *
 * @copyright Copyright (c) 2023
 *
 */

/*******************************************************************************
 *                              Includes     			                       *
 *******************************************************************************/

#include "../inc/bl_utils.h"
#include "../inc/bl_cmd_types.h"
#include <stddef.h>
#include <stdint.h>

/*******************************************************************************
 *                         	Public functions			                       *
 *******************************************************************************/

uint32_t bl_calculate_command_crc(void *command, uint32_t size) {
	const uint8_t *data = (const uint8_t*) command;
	uint32_t crc = 0xFFFFFFFF;
	uint32_t crc_offset = offsetof(BL_CommandHeader_t, CRC32);

	// Calculate CRC for the command (excluding the CRC field)
	for (uint32_t i = 0; i < size; i++) {
		if (i < crc_offset || i >= crc_offset + sizeof(uint32_t)) {
			crc ^= data[i];

			for (int j = 0; j < 8; j++) {
				crc = (crc >> 1) ^ ((crc & 1) * CRC32_POLY);
			}
		}
	}

	return ~crc;
}
