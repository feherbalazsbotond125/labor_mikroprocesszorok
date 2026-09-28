/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Labor 02 - GPIO Futófény és Pergésmentesített 4-bites számláló
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private define ------------------------------------------------------------*/
#define DEBOUNCING_TRESHOLD 20

/* Private variables ---------------------------------------------------------*/
uint8_t counter = 0;
uint8_t current_state_debounce = 0;
uint8_t previous_state_edge = 0;
uint8_t press_integrator = 0;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void display_counter(uint8_t val);
void running_light_demo(void);

/**
  * @brief  A program belépési pontja (main function)
  */
int main(void)
{
  /* MCU konfiguráció és perifériák inicializálása */
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  /* 1. Feladat: Induló futófény bemutató (balra és jobbra léptetés) */
  running_light_demo();

  /* A számláló kezdeti értékének (0) kiírása a LED-ekre */
  display_counter(counter);

  /* Főciklus */
  while (1)
  {
    /* 2. Feladat: Nyomógomb mintavételezése és szoftveres pergésmentesítése */
    GPIO_PinState raw_btn_state = HAL_GPIO_ReadPin(HMI_BTN_1_GPIO_Port, HMI_BTN_1_Pin);

    if (raw_btn_state == GPIO_PIN_SET)
    {
      if (press_integrator < DEBOUNCING_TRESHOLD)
      {
        press_integrator++;
      }
    }
    else
    {
      if (press_integrator > 0)
      {
        press_integrator--;
      }
    }

    // Stabil állapot frissítése a küszöbértékek alapján
    if (press_integrator >= DEBOUNCING_TRESHOLD)
    {
      current_state_debounce = 1;
    }
    else if (press_integrator == 0)
    {
      current_state_debounce = 0;
    }

    /* 3. Feladat: Lenyomási éldetektálás és 4-bites számláló léptetése */
    if (current_state_debounce && !previous_state_edge)
    {
      counter = (counter + 1) & 0x0F; // 0–15 tartomány (4 bit)
      display_counter(counter);
    }

    // Előző pergésmentesített állapot mentése a következő ciklus éldetektálásához
    previous_state_edge = current_state_debounce;

    /* 1 ms-os mintavételezési ütemezés */
    HAL_Delay(1);
  }
}

/**
  * @brief A számláló értékének (0–15) megjelenítése a 4 LED-en bitmaszkolással
  * @param val: A megjelenítendő bináris érték
  */
void display_counter(uint8_t val)
{
  HAL_GPIO_WritePin(HMI_LED_1_GPIO_Port, HMI_LED_1_Pin, (val & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(HMI_LED_2_GPIO_Port, HMI_LED_2_Pin, (val & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(HMI_LED_3_GPIO_Port, HMI_LED_3_Pin, (val & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(HMI_LED_4_GPIO_Port, HMI_LED_4_Pin, (val & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
  * @brief Futófény algoritmus a LED-ek egymás utáni be- és kikapcsolásához
  */
void running_light_demo(void)
{
  uint16_t led_pins[4] = {HMI_LED_1_Pin, HMI_LED_2_Pin, HMI_LED_3_Pin, HMI_LED_4_Pin};
  GPIO_TypeDef* led_ports[4] = {HMI_LED_1_GPIO_Port, HMI_LED_2_GPIO_Port, HMI_LED_3_GPIO_Port, HMI_LED_4_GPIO_Port};

  // Futófény balra
  for (int i = 0; i < 4; i++)
  {
    HAL_GPIO_WritePin(led_ports[i], led_pins[i], GPIO_PIN_SET);
    HAL_Delay(150);
    HAL_GPIO_WritePin(led_ports[i], led_pins[i], GPIO_PIN_RESET);
  }

  // Futófény jobbra
  for (int i = 2; i >= 0; i--)
  {
    HAL_GPIO_WritePin(led_ports[i], led_pins[i], GPIO_PIN_SET);
    HAL_Delay(150);
    HAL_GPIO_WritePin(led_ports[i], led_pins[i], GPIO_PIN_RESET);
  }
}

/**
  * @brief GPIO portok konfigurálása (LED kimenetek és Nyomógomb bemenet)
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Órajelek engedélyezése */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /* LED-ek kimeneti alapszintjének beállítása (kikapcsolva) */
  HAL_GPIO_WritePin(GPIOD, HMI_LED_1_Pin | HMI_LED_2_Pin | HMI_LED_3_Pin | HMI_LED_4_Pin, GPIO_PIN_RESET);

  /* LED lábak konfigurálása Push-Pull kimenetként */
  GPIO_InitStruct.Pin = HMI_LED_1_Pin | HMI_LED_2_Pin | HMI_LED_3_Pin | HMI_LED_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* Nyomógomb láb konfigurálása Pull-Down bemenetként */
  GPIO_InitStruct.Pin = HMI_BTN_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(HMI_BTN_1_GPIO_Port, &GPIO_InitStruct);
}
