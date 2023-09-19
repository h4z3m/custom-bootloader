/**
 * @file bl_comms.h
 * @author Hazem Montasser (h4z3m.private@gmail.com)
 * @brief   Bootloader communication layer
 * @version 0.1
 * @date 2023-08-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BL_COMMS_H
#define BL_COMMS_H

/*******************************************************************************
 *                              Includes                                       *
 *******************************************************************************/

#include "bl.h"
#include "bl_cmd_types.h"
#include <stdint.h>

/*******************************************************************************
 *                         Public functions prototypes                         *
 *******************************************************************************/

/**
 * @brief   Sends a response
 *
 * @param response  Response to be sent
 * @return BL_Status_t
 */
BL_Status_t BL_send_response(BL_Response *response);

/**
 * @brief   Sends an ack
 *
 * @param id        Command ID which is acked
 * @param ack_value Ack value (0 for no ack, 1 for ack)
 * @param nack_field Nack field (bitwise OR of the NACK fields)
 * @return BL_Status_t
 */
BL_Status_t BL_send_ack(BL_CommandID_t id, uint8_t ack_value,
		uint8_t nack_field);

/**
 * @fn BL_Status_t BL_receive_ack(void)
 * @brief 	Attempts to receive an acknowledgment from the host.
 *
 * @return BL_Status_OK	If ACK was received and no error fields
 * @return BL_Status_Error If no ACK was received or there was an error
 */
BL_Status_t BL_receive_ack(void);

/**
 * @fn BL_Status_t BL_send_packet(BL_DATA_PACKET_CMD*)
 * @brief
 *
 * @param packet
 * @return
 */
BL_Status_t BL_send_packet(BL_DATA_PACKET_CMD *packet);

/**
 * @brief   Receives an ack from the host
 *
 * @return BL_Status_t
 */
BL_Status_t BL_receive_ack();

#endif
