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

/*******************************************************************************
 *                         	Public functions			                       *
 *******************************************************************************/

void bl_handle_goto_addr_cmd(BL_GOTO_ADDR_CMD *cmd) {
	DEBUG_ASSERT(cmd != NULL);

	bl_debug_cmd_name(cmd->data.header.cmd_id);
	if (!VALIDATE_CMD(cmd->serialized_data, sizeof(BL_GOTO_ADDR_CMD),
			cmd->data.header.CRC32)) {
		BL_send_ack(cmd->data.header.cmd_id, 0, 0);
		return;
	}

	/* Send ACK back */
	BL_send_ack(cmd->data.header.cmd_id, 1, 0);
}

void bl_handle_mem_write_cmd(BL_MEM_WRITE_CMD *cmd) {
	DEBUG_ASSERT(cmd != NULL);

	bl_debug_cmd_name(cmd->data.header.cmd_id);

	if (!VALIDATE_CMD(cmd->serialized_data, sizeof(BL_MEM_WRITE_CMD),
			cmd->data.header.CRC32)) {
		BL_send_ack(cmd->data.header.cmd_id, 0, 2);
		return;
	}

	/* Send ACK back */
	BL_send_ack(cmd->data.header.cmd_id, 1, 0);
	BL_DATA_PACKET_CMD data_block = { 0 };

	uint32_t start_address = cmd->data.start_address;
	uint32_t total_bytes = 0;
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
			BL_send_ack(BL_ACK_CMD_ID, 0, 0);
			return;
		} else {
			total_bytes += data_block.data.data_len;

			DEBUG_INFO("Received valid data packet, length = %d bytes",
					data_block.data.data_len);

			/* TODO: write to memory */
			BL_flash_write(start_address, data_block.data.data_block,
					data_block.data.data_len);

			start_address += data_block.data.data_len;
			BL_send_ack(BL_ACK_CMD_ID, 1, 0);
		}
	}
	DEBUG_INFO("Total data received = %lu", total_bytes);
}

void bl_handle_mem_read_cmd(BL_MEM_READ_CMD *cmd) {
	DEBUG_ASSERT(cmd != NULL);

	bl_debug_cmd_name(cmd->data.header.cmd_id);
	if (!VALIDATE_CMD(cmd->serialized_data, sizeof(BL_MEM_READ_CMD),
			cmd->data.header.CRC32)) {
		BL_send_ack(BL_ACK_CMD_ID, 0, 0);
		return;
	}

	/* Send ACK back */
	BL_send_ack(cmd->data.header.cmd_id, 1, 0);
}

void bl_handle_ver_cmd(BL_VER_CMD *cmd) {
	DEBUG_ASSERT(cmd != NULL);

	bl_debug_cmd_name(cmd->data.header.cmd_id);
	if (!VALIDATE_CMD(cmd->serialized_data, sizeof(BL_VER_CMD),
			cmd->data.header.CRC32)) {
		DEBUG_WARN("Invalid CRC");
		BL_send_ack(cmd->data.header.cmd_id, 0, 0xFF);
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

	bl_debug_cmd_name(cmd->data.header.cmd_id);
	if (!VALIDATE_CMD(cmd->serialized_data, sizeof(BL_FLASH_ERASE_CMD),
			cmd->data.header.CRC32)) {
		DEBUG_WARN("Invalid CRC");
		BL_send_ack(cmd->data.header.cmd_id, 0, 0xFF);
		return;
	}

	/* Validate between page address & (page_count*page_size + page_address) */
	/* TODO Protect Bootloader code */
	if (!VALIDATE_ADDRESS_RANGE(cmd->data.page_number,
			(cmd->data.page_count) * BL_VS_PAGE_SIZE_BYTES + cmd->data.page_number,
			bl_ctx.AppLength)) {
		DEBUG_INFO("Invalid address range");
		BL_send_ack(cmd->data.header.cmd_id, 0, 1 | 2);
		return;
	}

	/* Send ACK back */
	BL_send_ack(cmd->data.header.cmd_id, 1, 0);

	DEBUG_INFO("Page number = 0x%08X", cmd->data.page_number);
	DEBUG_INFO("Page count = 0x%08X", cmd->data.page_count);

	/* Start erasing */
	BL_Status_t status = BL_erase_flash(cmd->data.page_number,
			cmd->data.page_count);

	DEBUG_INFO("Operation status: %d", status);
	/* Send ACK with operation status */
	BL_send_ack(cmd->data.header.cmd_id, status == BL_Status_OK, 0);
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
	BL_send_ack(cmd->data.header.cmd_id, cmd->data.key == BL_ENTER_CMD_MODE_KEY,
			0);
}
