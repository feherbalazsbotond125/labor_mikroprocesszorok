/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
#include "OLED.h"
#include "thermistor_misc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// display modes
enum Screen
{
	SCRN_Start = 0,
	SCRN_ADC = 1,
};

//display visualization modes
enum ADC_DisplayMode
{
	DM_Temp = 0,
	DM_Diff = 1,
	DM_ConvRes = 2
};

// number of display modes and visualization modes
const unsigned int NUMBER_OF_WORKING_MODES = 2;
const unsigned int NUMBER_OF_DISPLAY_MODES = 3;

// default display and visualization mode
unsigned short current_screen = SCRN_Start;
unsigned short display_mode = DM_Temp;

//variable to store the adc result
uint32_t adc_result = 0;

//variables to store the calculated temperature and correction
float temperature_calculated = 0.0;
float temperature_correction = 0.0;

// variable to store the state of the buttons
bool button_state = true;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// A-D conversion and show the current display
void ConvertAndDisplay()
{
	// variable to store the cumulated adc result
	float cumulated_result = 0.0;

	// number of the samples in the adc conversion
	const unsigned int SAMPLE_NUMBER = 16;
	// variable to check the success of the communication
	HAL_StatusTypeDef error_code = HAL_OK;
	for (unsigned int i = 0;i < SAMPLE_NUMBER;++i)
	{
		// start the analogue-digital conversion
		HAL_ADC_Start(&hadc1);
		error_code = HAL_ADC_PollForConversion(&hadc1, 100);
		if (error_code != HAL_OK)
		{
			DisplayTextAndInteger("Error:", error_code, NoUnit);
			HAL_Delay(1000);
			break;
		}
		cumulated_result += HAL_ADC_GetValue(&hadc1);
	}
	// check the success of the communication
	if (error_code != HAL_OK)
	{
		return;
	}
	// calculate the averaged adc value
	adc_result = cumulated_result/SAMPLE_NUMBER;
	// calculate the temperature from the adc result
	temperature_calculated = CalculateTemperature(adc_result);
	// temperature correction with the calculated correction value
	float temperature = temperature_calculated + temperature_correction;

	// show the result based on the actual visualization mode
	if (display_mode == DM_Temp)
	{
		DisplayTextAndFloat("Temp:", temperature, Celsius);
	}
	else if (display_mode == DM_Diff)
	{
		DisplayTextAndFloat("Diff:", temperature_correction, Celsius);
	}
	else if (display_mode == DM_ConvRes)
	{
		DisplayTextAndInteger("ADC:",  adc_result, NoUnit);
	}
}

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
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */

  OLED_SelectDisplay(Display_1);
  OLED_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // State machine
	  switch (current_screen)
	  {
	  case SCRN_Start:
		  DisplayText("Start", "screen", NoUnit);
		  HAL_Delay(500);
		  break;
	  case SCRN_ADC:
		  ConvertAndDisplay();
		  HAL_Delay(500);
		  break;
	  default:
		  DisplayText("Unknown", "screen", NoUnit);
		  HAL_Delay(500);
		  break;
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

//timer based button debouncing
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//start timer if the corresponding button has been pressed and no button press is in progress
	if ((GPIO_Pin == HMI_BTN_5_Pin || GPIO_Pin == HMI_BTN_4_Pin || GPIO_Pin == HMI_BTN_3_Pin || GPIO_Pin == HMI_BTN_2_Pin || GPIO_Pin == HMI_BTN_1_Pin) && button_state == true)
	{
		//start the timer
		HAL_TIM_Base_Start_IT(&htim1);
		//disable the buttons
		button_state = false;
	}
	else
	{
		__NOP();
	}
}

// button management
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// switching between screens
	if (HAL_GPIO_ReadPin(HMI_BTN_1_GPIO_Port, HMI_BTN_1_Pin) == GPIO_PIN_SET)
	{
		current_screen = (current_screen + 1) % NUMBER_OF_WORKING_MODES;
	}
	else if (HAL_GPIO_ReadPin(HMI_BTN_5_GPIO_Port, HMI_BTN_5_Pin) == GPIO_PIN_SET)
	{
		current_screen = (current_screen + NUMBER_OF_WORKING_MODES - 1) % NUMBER_OF_WORKING_MODES;
	}
	// switching between screen visualization modes
	else if (HAL_GPIO_ReadPin(HMI_BTN_4_GPIO_Port, HMI_BTN_4_Pin) == GPIO_PIN_SET && current_screen == SCRN_ADC)
	{
		display_mode = (display_mode + 1) % NUMBER_OF_DISPLAY_MODES;
	}
	else if (HAL_GPIO_ReadPin(HMI_BTN_2_GPIO_Port, HMI_BTN_2_Pin) == GPIO_PIN_SET && current_screen == SCRN_ADC)
	{
		display_mode = (display_mode + NUMBER_OF_DISPLAY_MODES - 1) % NUMBER_OF_DISPLAY_MODES;
	}
	// start the operation
	else if (HAL_GPIO_ReadPin(HMI_BTN_3_GPIO_Port, HMI_BTN_3_Pin) == GPIO_PIN_SET)
	{
		// start the calibration in Temp display mode
		if (current_screen == SCRN_ADC && display_mode == DM_Temp)
		{
			// store the difference between the right and the measured value
			temperature_correction = 0.0 - temperature_calculated;
		}
		// reset the correction value in Diff display mode
		else if (current_screen == SCRN_ADC && display_mode == DM_Diff)
		{
			temperature_correction = 0.0;
		}
	}

	// stop the timer
	HAL_TIM_Base_Stop_IT(&htim1);
	// button enable
	button_state = true;
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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
