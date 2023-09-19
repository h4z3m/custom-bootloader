/**
 * @file bl_handlers.c
 * @author Hazem Montasser (h4z3m.private@gmail.com)
 * @brief	Bootloader command handlers
 * @version 0.1
 * @date 2023-08-14
 *
 * @copyright Copyright (c) 2023
 *
 */

/*******************************************************************************
 *                              Includes                                       *
 *******************************************************************************/

#include "../inc/bl_handlers.h"
#include "../inc/bl.h"
#include "../inc/bl_cfg.h"
#include "../inc/bl_comms.h"
#include "../inc/bl_defs.h"
#include "../inc/bl_utils.h"
#include "LIB/DEBUG_UTILS.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
 *                              Definitions                                    *
 *******************************************************************************/

#define VALIDATE_CMD(data, length, crc) \
	(bl_calculate_command_crc(data, length) == crc)

/*******************************************************************************
 *                        Global Public variables                              *
 *******************************************************************************/

extern BL_Context_t bl_ctx;

/*******************************************************************************
 *                         	Private functions prototypes 					   *
 *******************************************************************************/

/**
 * @fn void bl_debug_cmd_name(BL_CommandID_t)
 * @brief	Prints the command name
 *
 * @param id	Command ID to be printed
 */
static void bl_debug_cmd_name(BL_CommandID_t id);

/**
 * @fn bool bl_is_address_outside_range(uint32_t, uint32_t, uint32_t)
 * @brief	Checks whether or not an address is outside the specified range.
 *
 * @param address		The address to be checked
 * @param startAddress	Start address of the range
 * @param endAddress	End address of the range
 * @return
 */
static bool bl_is_address_outside_range(uint32_t address, uint32_t startAddress,
		uint32_t endAddress);

/**
 * @fn bool bl_is_block_inside_range(uint32_t, uint32_t, uint32_t, uint32_t)
 * @brief 	Checks whether or not the given block of memory is within the
 * 	given range.
 *
 * @param startAddress		Start address of the range
 * @param endAddress		End addresss of the range
 * @param blockStartAddress	Start address for the block to be checked
 * @param blockSize			Block size
 * @return
 */
static bool bl_is_block_inside_range(uint32_t startAddress, uint32_t endAddress,
		uint32_t blockStartAddress, uint32_t blockSize);

/*******************************************************************************
 *                         	Private functions 			                       *
 *******************************************************************************/

static void bl_debug_cmd_name(BL_CommandID_t id) {
	switch (id) {
	case BL_GOTO_ADDR_CMD_ID:
		DEBUG_INFO("**** GO TO ADDR CMD ****");
		break;
	case BL_MEM_WRITE_CMD_ID:
		DEBUG_INFO("**** MEM WRITE CMD ****");
		break;
	case BL_MEM_READ_CMD_ID:
		DEBUG_INFO("**** MEM READ CMD ****");
		break;
	case BL_VER_CMD_ID:
		DEBUG_INFO("**** VER CMD ****");
		break;
	case BL_FLASH_ERASE_CMD_ID:
		DEBUG_INFO("**** FLASH ERASE CMD ****");
		break;
	case BL_ACK_CMD_ID:
		DEBUG_INFO("**** ACK CMD ****");
		break;
	case BL_JUMP_TO_APP_CMD_ID:
		DEBUG_INFO("**** JUMP TO APP CMD ****");
		break;
	case BL_ENTER_CMD_MODE_CMD_ID:
		DEBUG_INFO("**** ENTER CMD MODE CMD ****");
		break;
	default:
		DEBUG_INFO("Unknown command ID 0x%02X", id);
		break;
	}
}

static bool bl_is_address_outside_range(uint32_t address, uint32_t startAddress,
		uint32_t endAddress) {
	return ((address < startAddress) || (address > endAddress));
}

static bool bl_is_block_inside_range(uint32_t startAddress, uint32_t endAddress,
		uint32_t blockStartAddress, uint32_t blockSize) {
	uint32_t blockEndAddress = blockStartAddress + blockSize - 1;
	return (startAddress <= blockStartAddress)
			&& (blockEndAddress <= endAddress);
}

/*******************************************************************************
 *                         	Public functions			                       *
 *******************************************************************************/

void bl_handle_goto_addr_cmd(BL_GOTO_ADDR_CMD *cmd) {
	DEBUG_ASSERT(cmd != NULL);

	bl_debug_cmd_name(cmd->data.header.cmd_id);

	if (!VALIDATE_CMD(cmd->serialized_data, sizeof(BL_GOTO_ADDR_CMD),
			cmd->data.header.CRC32)) {
		DEBUG_WARN("Invalid CRC");
		BL_send_ack(cmd->data.header.cmd_id, 0, BL_NACK_INVALID_CRC);
		return;
	}

	/* Send ACK back */
	BL_send_ack(cmd->data.header.cmd_id, 1, 0);

	if (!bl_is_address_outside_range(cmd->data.address, bl_ctx.BL_startAddress,
			bl_ctx.BL_endAddress)) {
		DEBUG_WARN("Invalid address");
		BL_send_ack(cmd->data.header.cmd_id, 0, BL_NACK_INVALID_ADDRESS);
		return;
	}
	DEBUG_INFO("Setting current context address to 0x%x", cmd->data.address);
	bl_ctx.currentAddress = cmd->data.address;
}

void bl_handle_mem_write_cmd(BL_MEM_WRITE_CMD *cmd) {
	DEBUG_ASSERT(cmd != NULL);

	bl_debug_cmd_name(cmd->data.header.cmd_id);

	if (!VALIDATE_CMD(cmd->serialized_data, sizeof(BL_MEM_WRITE_CMD),
			cmd->data.header.CRC32)) {
		BL_send_ack(cmd->data.header.cmd_id, 0, BL_NACK_INVALID_CRC);
		return;
	}

	if (bl_is_block_inside_range(bl_ctx.BL_startAddress, bl_ctx.BL_endAddress,
			cmd->data.start_address, 1)) {
		BL_send_ack(cmd->data.header.cmd_id, 0, BL_NACK_INVALID_ADDRESS);
		return;
	}

	/* Send ACK back */
	BL_send_ack(cmd->data.header.cmd_id, 1, BL_NACK_SUCCESS);

	BL_DATA_PACKET_CMD data_block = { 0 };

	uint32_t start_address = cmd->data.start_address;
	uint32_t total_bytes = 0;
	uint32_t retries = 0;

	while (data_block.data.end_flag == 0) {

		/* Poll for the packet size */
		while (BL_receive((uint8_t*) &data_block.data.header,
				sizeof(data_block.data.header),
				BL_RECEIVE_TIMEOUT_MS) != BL_Status_OK)
			;

		/* Receive packet size bytes */
		if (data_block.data.header.payload_size
				&& data_block.data.header.payload_size
						<= BL_MAX_BUFFER_SIZE_BYTES) {
			while (BL_receive(
					&((uint8_t*) &data_block.data.header)[sizeof(BL_CommandHeader_t)],
					data_block.data.header.payload_size
							- sizeof(BL_CommandHeader_t),
					BL_RECEIVE_TIMEOUT_MS) != BL_Status_OK)
				;
		}

		if (!VALIDATE_CMD(data_block.serialized_data,
				data_block.data.header.payload_size,
				data_block.data.header.CRC32)) {
			DEBUG_ERROR("Data packet corrupted");
			BL_send_ack(data_block.data.header.cmd_id, 0,
					BL_NACK_INVALID_DATA | BL_NACK_INVALID_CRC);
			/* Force end flag to stay zero */
			data_block.data.end_flag = 0;
			if (retries >= BL_MAX_RETRIES) {
				return;
			}
			retries++;
			continue;
		} else if (bl_is_block_inside_range(bl_ctx.BL_startAddress,
				bl_ctx.BL_endAddress, start_address,
				data_block.data.data_len)) {
			/* If the incoming block will write to bootloader code, abort and send NACK */
			DEBUG_ERROR(
					"Conflict with bootloader address: Requested write to: (0x%08X to 0x%08X)",
					start_address, start_address + data_block.data.data_len);
			DEBUG_ERROR("Bootloader range: (0x%08X to 0x%08X)",
					bl_ctx.BL_startAddress, bl_ctx.BL_endAddress);
			/* Prevent overwrite of bootloader code */
			BL_send_ack(data_block.data.header.cmd_id, 0,
					BL_NACK_INVALID_ADDRESS);
			return;
		} else {
			DEBUG_INFO("Received valid data packet, length = %d bytes",
					data_block.data.data_len);

			total_bytes += data_block.data.data_len;
			/* Perform flash write */
			BL_flash_write(start_address, data_block.data.data_block,
					data_block.data.data_len);
			/* Increment the start address to point at the next block address */
			start_address += data_block.data.data_len;
			/* Send ACK on last operation */
			BL_send_ack(data_block.data.header.cmd_id, 1, BL_NACK_SUCCESS);
		}
	}

	DEBUG_INFO("Total data received = %lu", total_bytes);
}
BL_DATA_PACKET_CMD packet = { 0 };

void bl_handle_mem_read_cmd(BL_MEM_READ_CMD *cmd) {
	DEBUG_ASSERT(cmd != NULL);

	bl_debug_cmd_name(cmd->data.header.cmd_id);

	if (!VALIDATE_CMD(cmd->serialized_data, sizeof(BL_MEM_READ_CMD),
			cmd->data.header.CRC32)) {
		DEBUG_WARN("Invalid CRC");
		BL_send_ack(cmd->data.header.cmd_id, 0, BL_NACK_INVALID_CRC);
		return;
	}
	DEBUG_INFO("Start address = 0x%08X", cmd->data.start_addr);
	DEBUG_INFO("Read length = %d", cmd->data.length);

	/* Protect bootloader code against read-out */
	if (!bl_is_address_outside_range(cmd->data.start_addr,
			bl_ctx.BL_startAddress, bl_ctx.BL_endAddress)) {
		DEBUG_WARN("Attempting to read-out bootloader code");
		BL_send_ack(cmd->data.header.cmd_id, 0, BL_NACK_INVALID_ADDRESS);
		return;
	}

	/* Ensure range is not outside the flash memory */
	if (!bl_is_block_inside_range(BL_VS_FLASH_START_ADDRESS,
	BL_VS_FLASH_END_ADDRESS, cmd->data.start_addr, cmd->data.length)) {
		DEBUG_WARN("Attempting to read out of range memory");
		BL_send_ack(cmd->data.header.cmd_id, 0, BL_NACK_INVALID_ADDRESS);
		return;
	}

	/* Send ACK back */
	BL_send_ack(cmd->data.header.cmd_id, 1, BL_NACK_SUCCESS);

	uint32_t blocks = cmd->data.length / BL_DATA_BLOCK_SIZE;
	uint32_t remainderBytes = cmd->data.length % BL_DATA_BLOCK_SIZE;
	uint32_t startAddress = cmd->data.start_addr;
	uint32_t nextBlock = BL_DATA_BLOCK_SIZE;
	BL_Status_t status = BL_Status_OK;

	packet.data.data_len = BL_DATA_BLOCK_SIZE;
	packet.data.next_len = BL_DATA_BLOCK_SIZE;
	packet.data.header.cmd_id = BL_DATA_PACKET_CMD_ID;

	for (uint32_t i = 0; i < blocks; i++) {

		/* If this is the last even block, assign next block size to remainder bytes size */
		if (i == blocks - 1)
			nextBlock = remainderBytes;

		/* Copy the block */
		memcpy((uint8_t*) packet.data.data_block, (uint8_t*) startAddress,
		BL_DATA_BLOCK_SIZE);

		/* If this is the last packet, set the end flag */
		packet.data.end_flag = ((i + 1) * BL_DATA_BLOCK_SIZE)
				== cmd->data.length;

		if (nextBlock == 0) {
			packet.data.next_len = 0;
		} else {
			packet.data.next_len = sizeof(BL_DATA_PACKET_CMD)
					- BL_DATA_BLOCK_SIZE + nextBlock;
		}

		packet.data.header.payload_size = sizeof(BL_DATA_PACKET_CMD)
				- BL_DATA_BLOCK_SIZE + BL_DATA_BLOCK_SIZE;

		packet.data.header.CRC32 = bl_calculate_command_crc(&packet,
				packet.data.header.payload_size);

		BL_send_packet(&packet);

		/* Wait for ack on packet*/
		status = BL_receive_ack();

		if (status != BL_Status_OK) {
			return;
		}

		startAddress += (BL_DATA_BLOCK_SIZE);

	}

	while (remainderBytes) {

		/* Copy the remaining bytes */
		memcpy((uint8_t*) packet.data.data_block, (uint8_t*) startAddress,
				remainderBytes);

		/* Set the end flag as this is the last packet */
		packet.data.end_flag = 1;

		packet.data.header.payload_size = sizeof(BL_DATA_PACKET_CMD)
				- BL_DATA_BLOCK_SIZE + remainderBytes;

		packet.data.data_len = remainderBytes;

		packet.data.header.CRC32 = bl_calculate_command_crc(&packet,
				packet.data.header.payload_size);

		BL_send_packet(&packet);

		/* Wait for ack on last packet*/
		status = BL_receive_ack();

		if (status != BL_Status_OK) {
			// Re-send
			continue;
		} else {
			// Exit
			break;
		}
	}

}

void bl_handle_ver_cmd(BL_VER_CMD *cmd) {
	DEBUG_ASSERT(cmd != NULL);

	bl_debug_cmd_name(cmd->data.header.cmd_id);
	if (!VALIDATE_CMD(cmd->serialized_data, sizeof(BL_VER_CMD),
			cmd->data.header.CRC32)) {
		DEBUG_WARN("Invalid CRC");
		BL_send_ack(cmd->data.header.cmd_id, 0, BL_NACK_INVALID_CRC);
		return;
	}

	/* Send ACK back */
	BL_send_ack(cmd->data.header.cmd_id, 1, 0);

	/* Construct response */
	BL_Response response = { 0 };

	response.data.header.cmd_id = BL_RESPONSE_CMD_ID;
	response.data.header.payload_size = sizeof(BL_CommandHeader_t) + 1;
	response.data.data[0] = BL_VERSION;

	/* Must calculate CRC after setting all data */
	response.data.header.CRC32 = bl_calculate_command_crc(&response,
			response.data.header.payload_size);

	BL_send_response(&response);
}

void bl_handle_flash_erase_cmd(BL_FLASH_ERASE_CMD *cmd) {

	DEBUG_ASSERT(cmd != NULL);

	BL_NACK_t nack_field = BL_NACK_SUCCESS;

	bl_debug_cmd_name(cmd->data.header.cmd_id);
	if (!VALIDATE_CMD(cmd->serialized_data, sizeof(BL_FLASH_ERASE_CMD),
			cmd->data.header.CRC32)) {
		DEBUG_WARN("Invalid CRC");
		nack_field |= BL_NACK_INVALID_CRC;
		BL_send_ack(cmd->data.header.cmd_id, 0, nack_field);
	}

	/* Protect bootloader code against erase */
	if (!bl_is_address_outside_range(cmd->data.address, bl_ctx.BL_startAddress,
			bl_ctx.BL_endAddress)) {
		DEBUG_WARN("Attempting to erase bootloader code");
		DEBUG_WARN(
				"Conflict with bootloader address: Requested erase to: (0x%08X to 0x%08X)",
				cmd->data.address,
				cmd->data.address+cmd->data.page_count*BL_VS_PAGE_SIZE_BYTES);
		BL_send_ack(cmd->data.header.cmd_id, 0, BL_NACK_INVALID_ADDRESS);
		return;
	}

	/* Ensure range is not outside the flash memory */
	if (!bl_is_block_inside_range(BL_VS_FLASH_START_ADDRESS,
	BL_VS_FLASH_END_ADDRESS, cmd->data.address,
			cmd->data.page_count * BL_VS_PAGE_SIZE_BYTES)) {
		DEBUG_WARN("Attempting to erase out of range memory");
		DEBUG_WARN("Requested erase to: (0x%08X to 0x%08X)", cmd->data.address,
				cmd->data.address+cmd->data.page_count*BL_VS_PAGE_SIZE_BYTES);
		BL_send_ack(cmd->data.header.cmd_id, 0, BL_NACK_INVALID_ADDRESS);
		return;
	}

	DEBUG_INFO("Page number = 0x%08X", cmd->data.address);
	DEBUG_INFO("Page count = 0x%08X", cmd->data.page_count);

	BL_send_ack(cmd->data.header.cmd_id, nack_field == BL_NACK_SUCCESS,
			nack_field);

	if (nack_field != BL_NACK_SUCCESS)
		return;

	/* Start erasing */
	BL_Status_t status = BL_erase_flash(cmd->data.address,
			cmd->data.page_count);

	nack_field |= ((status == BL_Status_OK) ? 0 : BL_NACK_OPERATION_FAILURE);

	DEBUG_INFO("Operation status: %d", status);
	/* Send ACK with operation status */
	BL_send_ack(cmd->data.header.cmd_id, status == BL_Status_OK, nack_field);
}

void bl_handle_enter_cmd_mode_cmd(BL_ENTER_CMD_MODE_CMD *cmd) {
	DEBUG_ASSERT(cmd != NULL);

	bl_debug_cmd_name(cmd->data.header.cmd_id);
	if (!VALIDATE_CMD(cmd->serialized_data, sizeof(BL_ENTER_CMD_MODE_CMD),
			cmd->data.header.CRC32)) {
		DEBUG_WARN("Invalid CRC");
		BL_send_ack(cmd->data.header.cmd_id, 0, 0xFF);
		return;
	}

	if (cmd->data.key != BL_ENTER_CMD_MODE_KEY)
		bl_ctx.Mode = BL_Mode_default;

	/* Send ACK back if key matches */
	BL_send_ack(cmd->data.header.cmd_id, cmd->data.key == BL_ENTER_CMD_MODE_KEY,
			0);
}

void bl_handle_jump_to_app_cmd(BL_JUMP_TO_APP_CMD *cmd) {
	DEBUG_ASSERT(cmd != NULL);

	bl_debug_cmd_name(cmd->data.header.cmd_id);
	if (!VALIDATE_CMD(cmd->serialized_data, sizeof(BL_JUMP_TO_APP_CMD),
			cmd->data.header.CRC32)) {
		DEBUG_WARN("Invalid CRC");
		BL_send_ack(cmd->data.header.cmd_id, 0, 0xFF);
		return;
	}

	if (cmd->data.key == BL_JUMP_TO_APP_KEY)
		bl_ctx.Mode = BL_Mode_default;

	/* Send ACK back if key matches */
	BL_send_ack(cmd->data.header.cmd_id, cmd->data.key == BL_JUMP_TO_APP_KEY,
			0);
}
