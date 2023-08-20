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
 * @enum
 * @brief
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
 * @brief
 *
 * @param msec
 */
BL_WEAK void BL_delay(uint32_t msec);

/**
 * @fn BL_Status_t BL_initLED()
 * @brief
 *
 * @return
 */
BL_WEAK BL_Status_t BL_initLED();

/**
 * @fn BL_Status_t BL_initButton()
 * @brief
 *
 * @return
 */
BL_WEAK BL_Status_t BL_initButton();

/**
 * @fn BL_Status_t BL_initializeComm()
 * @brief
 *
 * @return
 */
BL_WEAK BL_Status_t BL_initComm();

/**
 * @fn uint8_t BL_GetButtonState()
 * @brief
 *
 * @return
 */
BL_WEAK uint8_t BL_GetButtonState();

/**
 * @fn uint8_t BL_SetLEDState()
 * @brief
 *
 * @return
 */
BL_WEAK void BL_SetLEDState(uint8_t state);

/**
 * @fn BL_Status_t BL_send(uint8_t*, uint8_t, uint32_t)
 * @brief
 *
 * @param data
 * @param len
 * @param timeout
 * @return
 */
BL_WEAK BL_Status_t BL_send(uint8_t *data, uint32_t len, uint32_t timeout);

/**
 * @fn BL_Status_t BL_receive(uint8_t*, uint8_t, uint32_t)
 * @brief
 *
 * @param data
 * @param len
 * @param timeout
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
