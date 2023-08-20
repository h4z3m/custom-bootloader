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

#include <stdint.h>
#include "bl.h"
#include "bl_cmd_types.h"

/*******************************************************************************
 *                         Public functions prototypes                         *
 *******************************************************************************/

/**
 * @brief
 *
 * @param response
 * @return BL_Status_t
 */
BL_Status_t BL_send_response(BL_Response *response);

/**
 * @brief
 *
 * @param id
 * @param ack_value
 * @return BL_Status_t
 */
BL_Status_t BL_send_ack(BL_CommandID_t id, uint8_t ack_value, uint8_t nack_field);

/**
 * @brief
 *
 * @return BL_Status_t
 */
BL_Status_t BL_receive_ack();

#endif
