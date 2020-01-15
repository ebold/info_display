/*
 * led_board.h
 *
 *  Created on: Jan 3, 2020
 *      Author: Enkhbold Ochirsuren
 */

#ifndef LED_BOARD_H_
#define LED_BOARD_H_

/**
 * @brief  External LED module control pins, OE = Output Enable, LDO = Load
 *
 * Arduino connector pins D7, D8 (PA.8, PA.9) are configured as GPIO and mapped to OE, LDO respectively.
 */

#define LED_EXT_OE_PIN          GPIO_PIN_8
#define LED_EXT_OE_GPIO_PORT    GPIOA

#define LED_EXT_LDO_PIN         GPIO_PIN_9
#define LED_EXT_LDO_GPIO_PORT   GPIOA

#define LED_EXT_OE_HIGH()       LED_EXT_OE_GPIO_PORT->BSRR = LED_EXT_OE_PIN
#define LED_EXT_OE_LOW()        LED_EXT_OE_GPIO_PORT->BRR = LED_EXT_OE_PIN

#define LED_EXT_LDO_HIGH()      LED_EXT_LDO_GPIO_PORT->BSRR = LED_EXT_LDO_PIN
#define LED_EXT_LDO_LOW()       LED_EXT_LDO_GPIO_PORT->BRR = LED_EXT_LDO_PIN

#define LED_OFF                 LED_EXT_OE_LOW()
#define LED_ON                  LED_EXT_OE_HIGH()
#define	PRE_LOAD                LED_EXT_LDO_HIGH()
#define	LOAD_BYTE               LED_EXT_LDO_LOW()

/**
 * @brief  External LED module SPI interface, SCK, SDO, SDI
 *
 * Arduino connector pins D13, D11, D12 (PA.5, PA.7, PA.6) are configured as SPI (SCK, MOSI, MISO) and
 * mapped to SCK, SDO, SDI signals.
 */

/**
 * @brief Functions to configure and manage the STM32F1xx resources (SPI2 and timers) used to drive
 *    		external LED module
 */

#define MAX_LED_BLK   3  // number of LED blocks on the LED display (1 block = 8x10 LEDs)
#define MAX_LED_COL   10 // columns of an LED block

#define CNT_1000      1000 // tick counts
#define CNT_100       100

enum
{
	TICK_100MS = 0, // 100ms ticks
	TICK_1000MS,    // 1000ms ticks
	N_TIMER_MS
} Timers_ms;

void initLedBoard(void); // initialize the LED board

uint8_t columnLedBlock;             // column index of 8x10 LED block
volatile uint8_t refreshLedBlocks;  // flag that indicates display refresh completion
uint8_t *pSource;                   // pointer to the current pattern

#endif /* LED_BOARD_H_ */
