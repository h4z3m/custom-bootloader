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

/*******************************************************************************
 *                              Definitions                                    *
 *******************************************************************************/

#define CRC32_POLY 0xEDB88320

/*******************************************************************************
 *                         	Public functions			                       *
 *******************************************************************************/

uint32_t crc32(const uint8_t *data, uint32_t length)
{
	uint32_t crc = 0xFFFFFFFF;

	for (uint32_t i = 0; i < length; i++)
	{
		crc ^= data[i];

		for (int j = 0; j < 8; j++)
		{
			crc = (crc >> 1) ^ ((crc & 1) * CRC32_POLY);
		}
	}

	return ~crc;
}

void erase_flash(uint32_t start_addr, uint32_t length, uint32_t erase_mask)
{
	while (--length)
		*((uint32_t *)start_addr++) = erase_mask;
}
