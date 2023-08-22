/**
 * @file bl_defs.h
 * @author Hazem Montasser (h4z3m.private@gmail.com)
 * @brief	Bootloader type definitions and macros used internally
 * @version 0.1
 * @date 2023-08-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef BL_DEFS_H_
#define BL_DEFS_H_

/*******************************************************************************
 *                              Includes                                       *
 *******************************************************************************/

#include <stdint.h>

/*******************************************************************************
 *							Definitions						        		   *
 *******************************************************************************/

#define BL_SYNC_BYTE_VALUE (0xA5)

#define BL_FLASH_ERASED_STATE_1 (0xFFFFFFFF)
#define BL_FLASH_ERASED_STATE_2 (0x00000000)
#define BL_COMMAND_TIMEOUT_MS (1000000U)
#define BL_RECEIVE_TIMEOUT_MS (1000U)
#define BL_SEND_TIMEOUT_MS (1000U)

/*******************************************************************************
 *							Type declarations  				        		   *
 *******************************************************************************/

/**
 * @enum
 * @brief
 *
 */
typedef enum
{
	BL_AppState_Valid,	/**< BL_AppState_Valid */
	BL_AppState_Invalid /**< BL_AppState_Invalid */
} BL_AppState_t;

typedef enum
{
	BL_Mode_init,
	BL_Mode_default,
	BL_Mode_cmd,
	BL_Mode_syncComplete,
	BL_Mode_receiveCommand
} BL_Mode_t;

typedef struct
{
	volatile uint32_t _MSP;				  /**< Main stack pointer */
	volatile void (*_ResetHandler)(void); /**< Reset handler function pointer*/
} BL_AppIVT_t;

typedef struct
{
	uint32_t *currentAddress;
	uint32_t *AppStartAddress;
	uint32_t *AppEndAddress;
	uint32_t AppLength;
	uint32_t eraseLength;
	BL_Mode_t Mode;
	struct CommandBuffer
	{
		BL_CommandHeader_t header;
		uint8_t buff[300];
	} CommandBuffer;
} BL_Context_t;

#endif /* BL_DEFS_H_ */
