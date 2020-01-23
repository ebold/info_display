/*
 * led_board.c
 *
 *  Created on: Jan 3, 2020
 *      Author: Enkhbold Ochirsuren
 */


#include "main.h"
#include "led_board.h"
#include "display.h"

volatile uint32_t ticks;                 // tick counter
volatile uint8_t cntSpiTransfer;         // SPI transfer counter

/**
 * initialize the LED board
 */
void initLedBoard(void)
{
	LED_OFF;                            // turn off all LED blocks

	ticks = 0;                          // reset tick counters

	refreshLedBlocks = FALSE;           // reset LED block refresh flag
	cntSpiTransfer = 0;                 // reset SPI transfer counter
	columnLedBlock = 0;                 // select the first column of each LED block
}

/**
 * SysTick interrupt handler. The handler is called every 1ms.
 */
void HAL_SYSTICK_Callback(void)
{
	event |= EVNT_1MS;    // 1 ms is over

	if (refreshLedBlocks) // start non-blocking SPI transfer with interrupt
	{
		refreshLedBlocks = FALSE; // reset flag
		LED_OFF;                  // turn off the LED matrix
		PRE_LOAD;                 // prepare to load transmitted bytes to the external latches

		cntSpiTransfer = 0;             // reset transfer counter

		pSource = pPattern + columnLedBlock;     // get byte-pattern to transmit and
		HAL_SPI_Transmit_IT(&hspi1, pSource, 1); // send it
	}
}

/**
 * SPI transmit completed callback function. Called after SPI transfer completion.
 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	++cntSpiTransfer;

	if (cntSpiTransfer < MAX_LED_BLK)           // byte-pattern transfer
	{
		pSource += MAX_LED_COL;		            // get next byte-pattern and
		HAL_SPI_Transmit_IT(hspi, pSource, 1);	// send it
	}
	else if (cntSpiTransfer == MAX_LED_BLK)
	{
		HAL_SPI_Transmit_IT(hspi, &columnLedBlock, 1); // send selected column
		/**
		 * Reminder: Get next column only if SPI transfer is completed!
		 * Otherwise next column has been sent instead of current column (non-blocking transfer).
		 */
	}
	else
	{
		++columnLedBlock;					// get next column
		if (columnLedBlock == MAX_LED_COL)	// wrap column starting from the first column
			columnLedBlock = 0;

		LOAD_BYTE;	// load transmitted bytes to external latches
		LED_ON;		// turn on all LED blocks

		refreshLedBlocks = TRUE;  // ready for next refresh
	}
}
