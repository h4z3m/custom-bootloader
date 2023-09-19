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
 *                              Definitions                                    *
 *******************************************************************************/

/**
 * @brief	Maximum buffer size for bootloader
 *
 */
#define BL_MAX_BUFFER_SIZE_BYTES (1512U)

/**
 * @brief	Enter command mode key value
 *
 */
#define BL_ENTER_CMD_MODE_KEY (0x09B21FFC)
/**
 * @brief	Jump to application key value
 *
 */
#define BL_JUMP_TO_APP_KEY (0x4032AFE5)

/**
 * @def BL_MAX_RETRIES
 * @brief	Max retries that a host can try or re-send a sub-command data
 *
 */
#define BL_MAX_RETRIES (5)

/**
 * @brief	Maximum page size for bootloader (Vendor specific)
 *
 */
#define BL_VS_PAGE_SIZE_BYTES (1024U)

/**
 * @def BL_VS_FLASH_START_ADDRESS
 * @brief 	Start address of flash memory (Vendor specific)
 *
 */
#define BL_VS_FLASH_START_ADDRESS (0x08000000)

/**
 * @def BL_VS_FLASH_END_ADDRESS
 * @brief 	End address of flash memory (Vendor specific)
 *
 */
#define BL_VS_FLASH_END_ADDRESS (0x08007FFF)

#endif /* BL_CFG_H_ */
