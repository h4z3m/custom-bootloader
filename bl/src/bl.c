/**
 * @file bl.c
 * @author Hazem Montasser (h4z3m.private@gmail.com)
 * @brief	Bootloader source file
 * @version 0.1
 * @date 2023-08-14
 *
 * @copyright Copyright (c) 2023
 *
 */

/*******************************************************************************
 *                              Includes                                       *
 *******************************************************************************/

#include "../inc/bl.h"
#include "../inc/bl_cfg.h"
#include "../inc/bl_cmd_types.h"
#include "../inc/bl_defs.h"
#include "../inc/bl_handlers.h"
#include "../BluePill Drivers/CURT_NVIC/CURT_NVIC_headers/NVIC_reg.h"
#include "LIB/DEBUG_UTILS.h"

/*******************************************************************************
 *                        Global Public variables                              *
 *******************************************************************************/

__attribute__((section("BL_CONTEXT")))     BL_Context_t bl_ctx;

/*******************************************************************************
 *                         Private functions prototypes                        *
 *******************************************************************************/

/**
 * @fn BL_AppState_t __validate_app(uint32_t)
 * @brief 	Validates whether a user application is present in flash memory
 * or not.
 *
 * @param a_appAddr	The application start address (start of IVT)
 * @return	BL_AppState_Valid If the app is valid.
 * @return BL_AppState_Invalid If the app is invalid.
 */
static BL_AppState_t __validate_app(uint32_t a_appAddr);

/**
 * @fn void __flash_led(uint8_t)
 * @brief 	Flashes the user led a number of times
 *
 * @param flashes	Number of flashes
 * @param delay_ms	Delay in milliseconds between every LED toggle
 */
static void __flash_led(uint8_t flashes, uint32_t delay_ms);

/**
 * @fn BL_Status_t __init_system(void)
 * @brief 	Initializes bootloader specific hardware such as indicator
 * LED's, buttons, UART, etc...
 *
 * @return	BL_Status_t Representing the initialization status
 */
static BL_Status_t __init_system(void);

/**
 * @fn void __set_msp(uint32_t)
 * @brief Sets the main stack pointer to the given one
 *
 * @param a_msp	Main stack pointer value (must be word aligned)
 */
static inline void __set_msp(uint32_t a_msp);

/**
 * @fn void __init_ctx(void)
 * @brief
 *
 */
static void __init_ctx(void);

/**
 * @fn void BL_CommandTimeout(void)
 * @brief
 *
 */
static void BL_CommandTimeout(void);

/**
 * @fn void BL_HandleCommand(void*)
 * @brief
 *
 * @param buffer
 */
static void BL_HandleCommand(void *buffer);

/**
 * @fn void BL_SyncHost(uint8_t)
 * @brief
 *
 * @param byte
 */
static void BL_SyncHost(uint8_t byte);

/**
 * @fn void BL_WaitForCommand()
 * @brief
 *
 */
static void BL_WaitForCommand();

/**
 * @fn void BL_ValidateApp()
 * @brief
 *
 */
static void BL_ValidateApp();

/**
 * @fn void BL_StateMachine()
 * @brief
 *
 */
static void BL_StateMachine();

/*******************************************************************************
 *                         Private functions 								   *
 *******************************************************************************/

static BL_AppState_t __validate_app(uint32_t a_appAddr) {
	if (a_appAddr == BL_FLASH_ERASED_STATE_1
			|| a_appAddr == BL_FLASH_ERASED_STATE_2) {
		return BL_AppState_Invalid;
	} else {
		return BL_AppState_Valid;
	}
}

static void __flash_led(uint8_t flashes, uint32_t delay_ms) {
	for (uint8_t i = 0; i < flashes; i++) {
		BL_SetLEDState(1);
		BL_delay(delay_ms);
		BL_SetLEDState(0);
		BL_delay(delay_ms);
	}
}

static void __init_ctx(void) {
	/* Application specific data from linker script */
	extern uint32_t _AppStartAddr;
	extern uint32_t _AppEndAddr;
	extern uint32_t _AppLength;

	/* Save addresses & app length to boot-loader context */
	bl_ctx.AppStartAddress = &_AppStartAddr;
	bl_ctx.AppEndAddress = &_AppEndAddr;
	bl_ctx.AppLength = &_AppLength;

	for (uint32_t i = 0; i < sizeof(bl_ctx.CommandBuffer); i++) {
		((uint8_t*) &bl_ctx.CommandBuffer)[i] = 0;
	}

	bl_ctx.Mode = BL_Mode_init;
}

static BL_Status_t __init_system(void) {

	BL_Status_t status = BL_Status_OK;
	do {

		status = BL_initLED();
		DEBUG_ASSERT(status == BL_Status_OK);
		__flash_led(5, 50);

		status = BL_initButton();
		DEBUG_ASSERT(status == BL_Status_OK);
		__flash_led(5, 50);

		DEBUG_INFO("Initialized GPIO successfully");

		status = BL_initComm();
		DEBUG_ASSERT(status == BL_Status_OK);
		__flash_led(5, 50);

		DEBUG_INFO("Initialized communication stack successfully");
	} while (0);

	return status;
}

static inline void __set_msp(uint32_t a_msp) {
	__asm volatile("MSR msp, %0" : : "r"(a_msp) :);
}

static void BL_CommandTimeout(void) {
	DEBUG_WARN("Timed out while waiting for a command");
	bl_ctx.Mode = BL_Mode_default;
}

static void BL_HandleCommand(void *buffer) {

	BL_CommandHeader_t *ptr = buffer;
	switch (ptr->cmd_id) {
	case BL_GOTO_ADDR_CMD_ID:
		// Handle BL_GOTO_ADDR_CMD_ID command
		bl_handle_goto_addr_cmd((BL_GOTO_ADDR_CMD*) buffer);
		break;

	case BL_MEM_WRITE_CMD_ID:
		// Handle BL_MEM_WRITE_CMD_ID command
		bl_handle_mem_write_cmd((BL_MEM_WRITE_CMD*) buffer);
		break;

	case BL_MEM_READ_CMD_ID:
		// Handle BL_MEM_READ_CMD_ID command
		bl_handle_mem_read_cmd((BL_MEM_READ_CMD*) buffer);
		break;

	case BL_VER_CMD_ID:
		// Handle BL_VER_CMD_ID command
		bl_handle_ver_cmd((BL_VER_CMD*) buffer);
		break;

	case BL_FLASH_ERASE_CMD_ID:
		// Handle BL_FLASH_ERASE_CMD_ID command
		bl_handle_flash_erase_cmd((BL_FLASH_ERASE_CMD*) buffer);
		break;

	case BL_ENTER_CMD_MODE_CMD_ID:
		// Handle BL_ENTER_CMD_MODE_CMD command
		bl_handle_enter_cmd_mode_cmd((BL_ENTER_CMD_MODE_CMD*) buffer);
		break;

	case BL_JUMP_TO_APP_CMD_ID:
		// Handle BL_JUMP_TO_APP_CMD command
		bl_handle_jump_to_app_cmd((BL_JUMP_TO_APP_CMD*) buffer);
		break;

	default:
		// Handle unknown command
		DEBUG_ERROR("Unknown command ID received: 0x%02X",
				((BL_CommandHeader_t* )buffer)->cmd_id);
		break;
	}
}

static void BL_SyncHost(uint8_t byte) {

	if (byte == BL_SYNC_BYTE_VALUE) {
		uint8_t sync_byte = BL_SYNC_BYTE_VALUE;
		DEBUG_INFO("Synchronized with host");
		BL_send(&sync_byte, 1, 100);
		bl_ctx.Mode = BL_Mode_cmd;
	} else {
		BL_receiveInterrupt(BL_SyncHost);
	}
}

static void BL_WaitForCommand() {

	while (bl_ctx.Mode == BL_Mode_receiveCommand)
		;

	/* Poll for the packet size */
	while (BL_receive((uint8_t*) &bl_ctx.CommandBuffer.header,
			sizeof(bl_ctx.CommandBuffer.header),
			BL_RECEIVE_TIMEOUT_MS) != BL_Status_OK && bl_ctx.Mode == BL_Mode_cmd)
		;
	/* If mode changed due to timeout, exit */
	if (bl_ctx.Mode != BL_Mode_cmd)
		return;

	/* Receive packet size bytes */
	if (bl_ctx.CommandBuffer.header.payload_size
			&& bl_ctx.CommandBuffer.header.payload_size
					<= BL_MAX_BUFFER_SIZE_BYTES) {
		while (BL_receive((uint8_t*) &bl_ctx.CommandBuffer.buff,
				bl_ctx.CommandBuffer.header.payload_size
						- sizeof(BL_CommandHeader_t),
				BL_RECEIVE_TIMEOUT_MS) != BL_Status_OK
				&& bl_ctx.Mode == BL_Mode_cmd)
			;
	}

	BL_disableTimeout();
	bl_ctx.Mode = BL_Mode_cmd;
	BL_HandleCommand((void*) &bl_ctx.CommandBuffer);
}

static void BL_ValidateApp() {

	/* Cast the application start address to AppIVT struct
	 * to access the start location and MSP*/
	const BL_AppIVT_t *const _appIVT = (BL_AppIVT_t*) (bl_ctx.AppStartAddress);

	/* TODO: validate app in a more systematic way, this could lead to bugs */
	/* Check if there is an application loaded by checking the first address */
	BL_AppState_t appState = __validate_app(*bl_ctx.AppStartAddress);

	switch (appState) {
	case BL_AppState_Invalid:
		DEBUG_WARN("No application found. Entering infinite loop...");
		while (1)
			;
		break;
	case BL_AppState_Valid:
		DEBUG_INFO("Application found");
		DEBUG_INFO("Setting MSP to %x", _appIVT->_MSP);
		DEBUG_INFO("Jumping to application at %x", _appIVT->_ResetHandler);

		/* TODO: Make these steps more generic to fit any MCU? */

		/* Change the vector table offset */
		SCB->VTOR = bl_ctx.AppStartAddress;

		/* Set the main stack pointer to the application's */
		__set_msp(_appIVT->_MSP);

		/* Jump to the reset handler of the application */
		_appIVT->_ResetHandler();

		break;
	default:
		DEBUG_ERROR("Un-handled state");
		break;
	}
}


static void BL_StateMachine() {
	__init_ctx();
	for (;;) {
		switch (bl_ctx.Mode) {
		case BL_Mode_init: {
			/* Initialize system and peripherals */
			BL_Status_t status = __init_system();

			DEBUG_ASSERT(status == BL_Status_OK);
			DEBUG_INFO("System initialization complete");

			/* BL is now waiting for command */
			bl_ctx.Mode = BL_Mode_receiveCommand;

			/* TODO: Check if the button is pressed or if there is a received command
			 */
		}
			break;
		case BL_Mode_receiveCommand: {

			/* Start timeout if no command was received */
			DEBUG_INFO("Starting timeout %u ms for receiving command",
					BL_COMMAND_TIMEOUT_MS);

			/* Synchronize with host before receiving a command */
			BL_receiveInterrupt(BL_SyncHost);

			BL_setTimeout(BL_COMMAND_TIMEOUT_MS, BL_CommandTimeout);

			DEBUG_INFO("Waiting for command");

			BL_WaitForCommand();

			if (bl_ctx.Mode == BL_Mode_cmd)
				break;

			DEBUG_INFO("No command received, searching for user application");
			bl_ctx.Mode = BL_Mode_default;
		}
			break;
		case BL_Mode_default: {
			BL_ValidateApp();
		}
			break;
		case BL_Mode_cmd: {
			DEBUG_INFO("Waiting for command");
			BL_WaitForCommand();
		}
			break;
		default:
			break;
		}
	}
}

/*******************************************************************************
 *                         Public functions *
 *******************************************************************************/

void BL_main() {

	BL_StateMachine();
	while (1)
		;
}
