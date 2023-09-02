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

/**
 * @brief     Maximum buffer size for bootloader
 *
 */
#define BL_MAX_BUFFER_SIZE_BYTES (300U)

/**
 * @brief     Maximum page size for bootloader (Vendor specific)
 *
 */
#define BL_VS_PAGE_SIZE_BYTES (1024U)

/**
 * @brief   Enter command mode key value
 *
 */
#define BL_ENTER_CMD_MODE_KEY (0x09B21FFC)
/**
 * @brief   Jump to application key value
 *
 */
#define BL_JUMP_TO_APP_KEY (0x4032AFE5)

#endif /* BL_CFG_H_ */
