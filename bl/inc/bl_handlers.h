/**
 * @file bl_handlers.h
 * @author Hazem Montasser (h4z3m.private@gmail.com)
 * @brief   Bootloader command handlers
 * @version 0.1
 * @date 2023-08-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BL_HANDLERS_H
#define BL_HANDLERS_H

/*******************************************************************************
 *                              Includes                                       *
 *******************************************************************************/

#include "bl_cmd_types.h"

/*******************************************************************************
 *                         Public functions prototypes                         *
 *******************************************************************************/

void bl_handle_goto_addr_cmd(BL_GOTO_ADDR_CMD *cmd);
void bl_handle_mem_write_cmd(BL_MEM_WRITE_CMD *cmd);
void bl_handle_mem_read_cmd(BL_MEM_READ_CMD *cmd);
void bl_handle_ver_cmd(BL_VER_CMD *cmd);
void bl_handle_flash_erase_cmd(BL_FLASH_ERASE_CMD *cmd);
void bl_handle_enter_cmd_mode_cmd(BL_ENTER_CMD_MODE_CMD *cmd);
void bl_handle_jump_to_app_cmd(BL_JUMP_TO_APP_CMD *cmd);

#endif
