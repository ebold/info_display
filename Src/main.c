/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "display.h"
#include "led_board.h"
#include "datetime.h"
#include "string.h"
/**
 * External LED module control pins, OE = Output Enable, LDO = Load
 * Arduino connector pins D7, D8 (PA.8, PA.9) are configured as GPIO and mapped to OE, LDO respectively.
 *
 * External LED module SPI interface, SCK, SDO, SDI
 * Arduino connector pins D13, D11, D12 (PA.5, PA.7, PA.6) are configured as SPI (SCK, MOSI, MISO) and
 * mapped to SCK, SDO, SDI signals.
 *
 * Connection diagram: Linux host <-- USB --> STM32-Nucleo <-- SPI/GPIO --> LED board
 */
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LEN_HMS      8    // length of HH:MM:SS
#define ASCII_DIGIT  0x30

#define CNT_1000     1000 // tick counts
#define CNT_100      100
#define CNT_10       10
#define CNT_3        3

typedef enum
{
	TICK_3MS = 0,   // 3ms ticks
	TICK_100MS,     // 100ms ticks
	TICK_1000MS,    // 1000ms ticks
	N_TIMER_MS
} tick_t;


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
volatile uint16_t event = 0;
uint8_t rx_byte[LEN_HMS]; // received byte buffer
uint8_t rx_cnt = 0;
uint8_t text[MAX_CHAR] = "Hello"; // info display buffer
uint8_t newline[2] = "\n\r";

button_t usrBtnB1 = BTN_RELEASED;

uint32_t cntTicks[N_TIMER_MS];       // tick counters
uint8_t  cntBtnStates[N_BTN_STATES]; // button state counters
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void displayDateTime(uint8_t *pText);
static void blinkChar(uint8_t *pText, uint8_t pos, uint8_t blink);
static void syncDateTime(struct mydatetime *loc_time, uint8_t *rx_buf);
static void echoToSender(void);
static void updateButtonState(button_t *button, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
/* USER CODE BEGIN PFP */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */


	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_SPI1_Init();
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	/* USER CODE BEGIN 2 */

	initLedBoard();
	initDisplay();
	displayText(DISP_MODE_STATIC, text, 0, MAX_CHAR);
	refreshLedBlocks = TRUE;	// enable to display data in text[]

	HAL_UART_Transmit(&huart2, text, sizeof(text), 0xFFFF); // print text[]
	HAL_UART_Transmit(&huart2, newline, sizeof(newline), 0xFFFF);

	HAL_UART_Receive_IT(&huart2, rx_byte, sizeof(rx_byte));

	HAL_Delay(1000);
	event |= EVNT_ENABLED | EVNT_DATETIME | EVNT_BLINK;
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		if (event & EVNT_ENABLED)
		{
			if (event & EVNT_1MS) {
				event &= ~EVNT_1MS;
				++cntTicks[TICK_3MS];
				if (cntTicks[TICK_3MS] >= CNT_3) {
					cntTicks[TICK_3MS] = 0;
					updateButtonState(&usrBtnB1, GPIOC, GPIO_PIN_13); // read the on-board user button (B1)
					setTime(usrBtnB1, text);  // set datetime depending on the user button state
				}

				++cntTicks[TICK_100MS];
				if (cntTicks[TICK_100MS] >= CNT_100) {
					cntTicks[TICK_100MS] = 0;
					tickDateTime();  // tick local datetime periodically
				}
			}

			if (event & EVNT_BLINK) {
				event &= ~EVNT_BLINK;
				if (!isSetTimeActive())      // check if setting datetime is in progress
					blinkChar(text, 2, ':'); // toggle ':' in datetime
			}
			else if (event & EVNT_DATETIME) {
				event &= ~EVNT_DATETIME;
				displayDateTime(text);  // display datetime
			}
			else if (event & EVNT_UART_RX)
			{
				event &= ~EVNT_UART_RX;
				syncDateTime(&s_mydatetime, rx_byte); // synchronize local datetime

				echoToSender(); // send the received data back to sender
			}
		}
		/* USER CODE END WHILE */
		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void)
{

	/* USER CODE BEGIN SPI1_Init 0 */

	/* USER CODE END SPI1_Init 0 */

	/* USER CODE BEGIN SPI1_Init 1 */

	/* USER CODE END SPI1_Init 1 */
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_1LINE;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; // 1/8 = 1MHz
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN SPI1_Init 2 */

	/* USER CODE END SPI1_Init 2 */

}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);

	/*Configure GPIO pin : PC13 - on-board user button */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : PB5 - CN9.5 - D4*/
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : PA8, PA9 */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART2) {
		HAL_UART_Receive_IT(&huart2, rx_byte, sizeof(rx_byte)); // receive data, reset interrupt
		event |= EVNT_UART_RX;
	}
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

/**
 * @brief	Synchronize local datetime with the RX data
 * @param	loc_time - variable for local datetime, rx_buf - RX data buffer
 * @retval	None
 */
void syncDateTime(struct mydatetime *loc_time, uint8_t *rx_buf)
{
	uint8_t digit_10 = *(rx_buf);
	digit_10 &= ~ASCII_DIGIT;
	uint8_t digit_1 = *(rx_buf + 1);
	digit_1 &= ~ASCII_DIGIT;
	loc_time->hour = digit_10 * 10 + digit_1;
	digit_10 = *(rx_buf + 3);
	digit_10 &= ~ASCII_DIGIT;
	digit_1 = *(rx_buf + 4);
	digit_1 &= ~ASCII_DIGIT;
	loc_time->minute = digit_10 * 10 + digit_1;
	digit_10 = *(rx_buf + 6);
	digit_10 &= ~ASCII_DIGIT;
	digit_1 = *(rx_buf + 7);
	digit_1 &= ~ASCII_DIGIT;
	loc_time->second = digit_10 * 10 + digit_1;
	event |= EVNT_DATETIME;
}

/**
 * @breif	Write datetime value to a dedicated display buffer
 */
void displayDateTime(uint8_t *pText)
{
	uint8_t i = s_mydatetime.hour / 10;
	if (i)
		*pText = i | ASCII_DIGIT;
	else
		*pText = ' ';

	*(pText + 1) = (s_mydatetime.hour % 10) | ASCII_DIGIT;
	i = s_mydatetime.minute / 10;
	if (i)
	{
		*(pText + 3) = i | ASCII_DIGIT;
		*(pText + 4) = (s_mydatetime.minute % 10) | ASCII_DIGIT;
	}
	else
	{
		*(pText + 3) = (s_mydatetime.minute % 10) | ASCII_DIGIT;
		*(pText + 4) = ' ';
	}
	displayText(DISP_MODE_STATIC, pText, 0, 5);
}

/**
 * @breif	Blink a character in a text buffer
 */
void blinkChar(uint8_t *pText, uint8_t pos, uint8_t blink)
{
	uint8_t ch = *(pText + pos);

	if (ch == blink)
	{
		*(pText + pos) = ' ';
	}
	else
	{
		*(pText + pos) = blink;
	}

	displayText(DISP_MODE_STATIC, pText, pos, 1);
}

/**
 * @brief	Send the received data back to a sender
 */
void echoToSender(void)
{
	HAL_UART_Transmit(&huart2, rx_byte, sizeof(rx_byte), 0xFFFF);
	HAL_UART_Transmit(&huart2, newline, sizeof(newline), 0xFFFF);
}

/**
 * @brief	Read the on-board user button state
 * @param	*button	Pointer to button variable
 * @param	*GPIOx	Pointer to GPIO
 * @param	GPIO_Pin	GPIO pin
 * @retval	None
 */
void updateButtonState(button_t *button, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_PinState state = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);

	if (state == GPIO_PIN_RESET)	// button is pressed
	{
		cntBtnStates[BTN_PRESSED]++;
		cntBtnStates[BTN_RELEASED] = 0;
		if (cntBtnStates[BTN_PRESSED] >= CNT_10)
		{
			cntBtnStates[BTN_PRESSED] = CNT_10;
			*button = BTN_PRESSED;
			//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
		}
	}
	else	// button is released
	{
		cntBtnStates[BTN_RELEASED]++;
		cntBtnStates[BTN_PRESSED] = 0;
		if (cntBtnStates[BTN_RELEASED] >= CNT_10)
		{
			cntBtnStates[BTN_RELEASED] = CNT_10;
			*button = BTN_RELEASED;
			//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
		}
	}
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{ 
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
