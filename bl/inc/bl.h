/**
 * @file bl.h
 * @author Hazem Montasser (h4z3m.private@gmail.com)
 * @brief   Bootloader header file
 * @version 0.1
 * @date 2023-08-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BL_H_
#define BL_H_

/*******************************************************************************
 *                              Includes                                       *
 *******************************************************************************/

#include <stdint.h>

/*******************************************************************************
 *                              Definitions                                    *
 *******************************************************************************/

#define BL_VERSION (0x01)

#define BL_WEAK __attribute__((__weak__))

/*******************************************************************************
 *							Type declarations  				        		   *
 *******************************************************************************/

/**
 * @enum	BL_Status_t
 * @brief	Bootloader status enum
 *
 */
typedef enum {
	BL_Status_OK, /**< BL_Status_OK */
	BL_Status_Error /**< BL_Status_Error */
} BL_Status_t;

/*******************************************************************************
 *                         Weak public functions prototypes                    *
 *******************************************************************************/

/**
 * @fn void BL_delay(uint32_t)
 * @brief	Provides a millisecond polling delay
 *
 * @param msec	Number of milliseconds to delay
 */
BL_WEAK void BL_delay(uint32_t msec);

/**
 * @fn BL_Status_t BL_initLED()
 * @brief	Initializes the hardware indicator LED
 *
 * @return	BL_Status_OK	If initialized successfully
 * @return	BL_Status_Error	If did not initialize successfully
 */
BL_WEAK BL_Status_t BL_initLED();

/**
 * @fn BL_Status_t BL_initButton()
 * @brief	Initializes the hardware button to enter bootloader command mode
 *
 * @return	BL_Status_OK	If initialized successfully
 * @return	BL_Status_Error	If did not initialize successfully
 */
BL_WEAK BL_Status_t BL_initButton();

/**
 * @fn BL_Status_t BL_initializeComm()
 * @brief	Initializes the communication stack that will be used to send/receive data
 * 	from the host
 *
 * @return	BL_Status_OK	If initialized successfully
 * @return	BL_Status_Error	If did not initialize successfully
 */
BL_WEAK BL_Status_t BL_initComm();

/**
 * @fn uint8_t BL_GetButtonState()
 * @brief	Returns the current button state
 *
 * @return 0	If not pressed
 * @return Else If pressed
 */
BL_WEAK uint8_t BL_GetButtonState();

/**
 * @fn uint8_t BL_SetLEDState()
 * @brief	Sets the LED state.
 * 	- If state is zero, disable LED
 * 	- IF state is non-zero, enable LED
 *
 * @return
 */
BL_WEAK void BL_SetLEDState(uint8_t state);

/**
 * @fn BL_Status_t BL_send(uint8_t*, uint8_t, uint32_t)
 * @brief	Sends an array of bytes with specified length and returns after a timeout
 *
 * @param data		Data array
 * @param len		Length of the data in bytes
 * @param timeout	Timeout in milliseconds
 * @return
 */
BL_WEAK BL_Status_t BL_send(uint8_t *data, uint32_t len, uint32_t timeout);

/**
 * @fn BL_Status_t BL_receive(uint8_t*, uint8_t, uint32_t)
 * @brief	Receives an array of bytes with specified length and returns after a timeout
 *
 * @param data		Data array
 * @param len		Length of the data in bytes
 * @param timeout	Timeout in milliseconds
 * @return
 */
BL_WEAK BL_Status_t BL_receive(uint8_t *data, uint32_t len, uint32_t timeout);

/**
 * @fn BL_Status_t BL_receiveInterrupt(void(*)(uint8_t))
 * @brief
 *
 * @param callback
 * @return
 */
BL_WEAK BL_Status_t BL_receiveInterrupt(void (*callback)(uint8_t));

/**
 * @fn BL_Status_t BL_disableInterrupt(void)
 * @brief
 *
 * @return
 */
BL_WEAK BL_Status_t BL_disableInterrupt(void);

/**
 * @fn void BL_setTimeout(uint32_t, void(*)(void))
 * @brief
 *
 * @param msec
 * @param callback
 */
BL_WEAK void BL_setTimeout(uint32_t msec, void (*callback)(void));

/**
 * @fn void BL_disableTimeout(void)
 * @brief
 *
 */
BL_WEAK void BL_disableTimeout(void);

/**
 * @fn BL_Status_t BL_erase_flash(uint32_t, uint32_t)
 * @brief
 *
 * @param page_address
 * @param page_count
 * @return
 */
BL_WEAK BL_Status_t BL_erase_flash(uint32_t page_address, uint32_t page_count);

/**
 * @fn BL_Status_t BL_flash_write(uint32_t, uint8_t[], uint32_t)
 * @brief
 *
 * @param start_address
 * @param data
 * @param data_len
 * @return
 */
BL_WEAK BL_Status_t BL_flash_write(uint32_t start_address, uint8_t data[],
		uint32_t data_len);

/*******************************************************************************
 *                         Public functions prototypes                    	   *
 *******************************************************************************/

void BL_main();

#endif /* BL_H_ */
