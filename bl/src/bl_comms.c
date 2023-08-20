/**
 * @file bl_comms.c
 * @author Hazem Montasser (h4z3m.private@gmail.com)
 * @brief   Bootloader communication functions
 * @version 0.1
 * @date 2023-08-14
 *
 * @copyright Copyright (c) 2023
 *
 */

/*******************************************************************************
 *                              Includes                                       *
 *******************************************************************************/

#include <stdint.h>
#include "../inc/bl_comms.h"
#include "../inc/bl_defs.h"

/*******************************************************************************
 *                          Public functions                                   *
 *******************************************************************************/

BL_Status_t BL_receive_ack()
{
	BL_ACK ack = {0};

	BL_receive(ack.serialized_data, sizeof(ack), BL_RECEIVE_TIMEOUT_MS);

	if (ack.data.ack == 1 && ack.data.cmd_id == BL_ACK_CMD_ID)
	{
		return BL_Status_OK;
	}
	else
	{
		return BL_Status_Error;
	}
}

BL_Status_t BL_send_ack(BL_CommandID_t id, uint8_t ack_value, uint8_t nack_field)
{
	BL_ACK ack = {0};

	ack.data.cmd_id = id;
	ack.data.ack = ack_value;
	ack.data.field = nack_field;

	return BL_send(ack.serialized_data, sizeof(ack), BL_SEND_TIMEOUT_MS);
}

BL_Status_t BL_send_response(BL_Response *response)
{
	/* Data size + 4(CRC32) + 1 (Command ID) */
	BL_Status_t status = BL_send(response->serialized_data,
								 response->data.header.payload_size, BL_SEND_TIMEOUT_MS);

	return status;
}
