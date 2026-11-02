/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "gpio.h"
#include "boot_protocol.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bootloader.h"
#include "boot_commands.h"
#include "boot_crc.h"
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
volatile uint32_t teste = 0;
volatile uint32_t boot_stay = 0;
volatile Boot_Info boot_info = {0};
volatile uint32_t debug_command = 0U;
volatile Boot_CommandStatus debug_status = BOOT_CMD_OK;
volatile uint32_t debug_crc = 0U;
volatile uint32_t debug_crc_empty = 0U;
uint8_t debug_frame[BOOT_PROTOCOL_MAX_FRAME_SIZE] = {0};
volatile uint32_t debug_frame_size = 0U;
Boot_FrameView debug_frame_view = {0};

volatile Boot_FrameStatus debug_parse_status;
volatile Boot_FrameStatus debug_corrupt_status;
volatile Boot_FrameStatus debug_short_status;
volatile Boot_FrameStatus debug_length_status;

uint8_t debug_response[BOOT_PROTOCOL_MAX_FRAME_SIZE] = {0};

volatile uint32_t debug_response_size = 0U;
volatile Boot_FrameStatus debug_response_status;

Boot_FrameView debug_response_view = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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


/* USER CODE BEGIN 2 */
BootCommands_GetInfo(&boot_info);
app_vectors_ok = Bootloader_CheckApplicationVectors();
static const uint8_t crc_test_data[] = "123456789";

debug_crc = BootCRC_Calculate(
    crc_test_data,
    sizeof(crc_test_data) - 1U
);
debug_frame_size = (uint32_t)BootProtocol_BuildFrame(
    BOOT_CMD_GET_INFO,
    1U,
    NULL,
    0U,
    debug_frame,
    sizeof(debug_frame)
);
debug_parse_status = BootProtocol_ParseFrame(
    debug_frame,
    debug_frame_size,
    &debug_frame_view
);

/* Change one sequence bit without updating the CRC. */
debug_frame[4] ^= 0x01U;

Boot_FrameView corrupt_view = {0};

debug_corrupt_status = BootProtocol_ParseFrame(
    debug_frame,
    debug_frame_size,
    &corrupt_view
);

/* Restore the original frame. */
debug_frame[4] ^= 0x01U;
Boot_FrameView invalid_view = {0};

/* A frame shorter than the minimum must be rejected. */
debug_short_status = BootProtocol_ParseFrame(
    debug_frame,
    BOOT_PROTOCOL_MIN_FRAME_SIZE - 1U,
    &invalid_view
);

/* Declare 257 payload bytes, exceeding the 256-byte limit. */
debug_frame[6] = 0x01U;
debug_frame[7] = 0x01U;

debug_length_status = BootProtocol_ParseFrame(
    debug_frame,
    debug_frame_size,
    &invalid_view
);

/* Restore the original empty payload length. */
debug_frame[6] = 0U;
debug_frame[7] = 0U;

/* Refresh the validated request view. */
debug_parse_status = BootProtocol_ParseFrame(
    debug_frame,
    debug_frame_size,
    &debug_frame_view
);

if (debug_parse_status == BOOT_FRAME_OK)
{
    debug_response_size =
        (uint32_t)BootProtocol_BuildInfoResponse(
            &debug_frame_view,
            debug_response,
            sizeof(debug_response)
        );

    debug_response_status = BootProtocol_ParseFrame(
        debug_response,
        debug_response_size,
        &debug_response_view
    );
}
debug_crc_empty = BootCRC_Calculate(crc_test_data, 0U);
if ((boot_stay == 0U) && (app_vectors_ok == 1U))
{
    Bootloader_TryStartApplication();
}


/* USER CODE END 2 */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (debug_command != 0U)
{
    uint32_t command = debug_command;
    debug_command = 0U;

    debug_status = BootCommands_Execute(command, &boot_info);
}

teste++;
HAL_Delay(10);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
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

/* USER CODE BEGIN 4 */

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
#ifdef USE_FULL_ASSERT
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
