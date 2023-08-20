/**
 * @file bl_cfg.h
 * @author Hazem Montasser (h4z3m.private@gmail.com)
 * @brief   Bootloader static configuration file
 * @version 0.1
 * @date 2023-08-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BL_CFG_H_
#define BL_CFG_H_

/*******************************************************************************
 *                              Includes                                       *
 *******************************************************************************/

/*******************************************************************************
 *                              Definitions                                    *
 *******************************************************************************/
#define BL_MAX_BUFFER_SIZE_BYTES	(300U)
#define BL_VS_PAGE_SIZE_BYTES		(1024U)
#define BL_ENTER_CMD_MODE_KEY		(0x09B21FFC)
#define BL_JUMP_TO_APP_KEY			(0x4032AFE5)
#endif /* BL_CFG_H_ */
