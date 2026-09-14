#include "main.h"
#include "lcd_i2c.h"
#include <stdint.h>
#include <stdio.h>

/* Handles */
I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim4;

/* Function prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
static void MX_I2C1_Init(void);

void delay_us(uint16_t us);
uint32_t HCSR04_Read(void);
void Parking_Assist(uint32_t distance);
void All_Off(void);

/* Microsecond delay using TIM4 */
void delay_us(uint16_t us)
{
  __HAL_TIM_SET_COUNTER(&htim4, 0);
  while (__HAL_TIM_GET_COUNTER(&htim4) < us);
}

/* Read HC-SR04 distance */
uint32_t HCSR04_Read(void)
{
  uint32_t time_us = 0;
  uint32_t timeout = 30000;

  HAL_GPIO_WritePin(trig_GPIO_Port, trig_Pin, GPIO_PIN_RESET);
  delay_us(2);

  HAL_GPIO_WritePin(trig_GPIO_Port, trig_Pin, GPIO_PIN_SET);
  delay_us(10);
  HAL_GPIO_WritePin(trig_GPIO_Port, trig_Pin, GPIO_PIN_RESET);

  while (HAL_GPIO_ReadPin(echo_GPIO_Port, echo_Pin) == GPIO_PIN_RESET)
  {
    if (--timeout == 0)
      return 999;
  }

  __HAL_TIM_SET_COUNTER(&htim4, 0);

  while (HAL_GPIO_ReadPin(echo_GPIO_Port, echo_Pin) == GPIO_PIN_SET)
  {
    if (__HAL_TIM_GET_COUNTER(&htim4) > 30000)
      return 999;
  }

  time_us = __HAL_TIM_GET_COUNTER(&htim4);

  return time_us / 58U;
}

/* Turn everything off */
void All_Off(void)
{
  HAL_GPIO_WritePin(green_GPIO_Port, green_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(yellow_GPIO_Port, yellow_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, GPIO_PIN_RESET);
}

/* Parking assist logic */
void Parking_Assist(uint32_t distance)
{
  static uint32_t last_distance = 999;
  static uint32_t unchanged_start_time = 0;
  static uint8_t muted = 0;
  static uint32_t last_toggle_time = 0;
  static uint8_t led_state = 0;

  uint32_t now = HAL_GetTick();

  // Mute logic (unchanged)
  if (distance == 999)
  {
    All_Off();
    return;
  }

  if (distance == last_distance)
  {
    if ((now - unchanged_start_time) > 3000)
    {
      muted = 1;
    }
  }
  else
  {
    last_distance = distance;
    unchanged_start_time = now;
    muted = 0;
  }

  // Handle different distance ranges
  if (distance > 30)
  {
    // GREEN LED - SOLID
    All_Off();
    HAL_GPIO_WritePin(green_GPIO_Port, green_Pin, GPIO_PIN_SET);
  }
  else if (distance > 10 && distance <= 30)
  {
    // YELLOW LED - SLOW BLINK (500ms)
    if (now - last_toggle_time >= 500)
    {
      led_state = !led_state;
      last_toggle_time = now;
    }

    All_Off();
    if (led_state)
    {
      HAL_GPIO_WritePin(yellow_GPIO_Port, yellow_Pin, GPIO_PIN_SET);
      if (!muted)
        HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, GPIO_PIN_SET);
    }
  }
  else if (distance > 5 && distance <= 10)
  {
    // RED LED - FAST BLINK (150ms)
    if (now - last_toggle_time >= 150)
    {
      led_state = !led_state;
      last_toggle_time = now;
    }

    All_Off();
    if (led_state)
    {
      HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, GPIO_PIN_SET);
      if (!muted)
        HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, GPIO_PIN_SET);
    }
  }
  else  // distance <= 5
  {
    // RED LED - SOLID (no blink)
    All_Off();
    HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, GPIO_PIN_SET);
    if (!muted)
      HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, GPIO_PIN_SET);
  }
}

int main(void)
{
  uint32_t distance = 0;
  char lcd_buf[16];

  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_TIM4_Init();
  MX_I2C1_Init();

  HAL_TIM_Base_Start(&htim4);

  lcd_init(&hi2c1);
  lcd_clear();
  lcd_put_cur(0, 0);
  lcd_send_string("Parking Assist");
  HAL_Delay(1000);

  while (1)
  {
    distance = HCSR04_Read();
    Parking_Assist(distance);

    lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("Distance:");

    lcd_put_cur(1, 0);

    if (distance == 999)
    {
      lcd_send_string("No Echo");
    }
    else
    {
      sprintf(lcd_buf, "%lu cm", distance);
      lcd_send_string(lcd_buf);
    }

    HAL_Delay(300);
  }
}

/* System clock */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* I2C1 for LCD */
static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* TIM4 for microsecond delay */
static void MX_TIM4_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 84 - 1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/* GPIO setup */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOA, trig_Pin | buzzer_Pin | RED_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, green_Pin | yellow_Pin, GPIO_PIN_RESET);

  /* TRIG = OUTPUT */
  GPIO_InitStruct.Pin = trig_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(trig_GPIO_Port, &GPIO_InitStruct);

  /* ECHO = INPUT */
  GPIO_InitStruct.Pin = echo_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(echo_GPIO_Port, &GPIO_InitStruct);

  /* BUZZER + RED = OUTPUT */
  GPIO_InitStruct.Pin = buzzer_Pin | RED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* GREEN + YELLOW = OUTPUT */
  GPIO_InitStruct.Pin = green_Pin | yellow_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
