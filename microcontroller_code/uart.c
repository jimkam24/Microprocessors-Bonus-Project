
/**
  ******************************************************************************
  * @file    app_x-cube-ai.c
  * @author  X-CUBE-AI C code generator
  * @brief   AI program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

 /*
  * Description
  *   v1.0 - Minimum template to show how to use the Embedded Client API
  *          model. Only one input and one output is supported. All
  *          memory resources are allocated statically (AI_NETWORK_XX, defines
  *          are used).
  *          Re-target of the printf function is out-of-scope.
  *   v2.0 - add multiple IO and/or multiple heap support
  *
  *   For more information, see the embeded documentation:
  *
  *       [1] %X_CUBE_AI_DIR%/Documentation/index.html
  *
  *   X_CUBE_AI_DIR indicates the location where the X-CUBE-AI pack is installed
  *   typical : C:\Users\<user_name>\STM32Cube\Repository\STMicroelectronics\X-CUBE-AI\7.1.0
  */

  #ifdef __cplusplus
  extern "C" {
 #endif

 /* Includes ------------------------------------------------------------------*/

 #if defined ( __ICCARM__ )
 #elif defined ( __CC_ARM ) || ( __GNUC__ )
 #endif

 /* System headers */
 #include <stdint.h>
 #include <stdlib.h>
 #include <stdio.h>
 #include <inttypes.h>
 #include <string.h>

 #include "app_x-cube-ai.h"
 #include "main.h"
 #include "ai_datatypes_defines.h"
 #include "network.h"
 #include "network_data.h"

 /* USER CODE BEGIN includes */
 /* USER CODE END includes */

 /* IO buffers ----------------------------------------------------------------*/

 #if !defined(AI_NETWORK_INPUTS_IN_ACTIVATIONS)
 AI_ALIGNED(4) ai_i8 data_in_1[AI_NETWORK_IN_1_SIZE_BYTES];
 ai_i8* data_ins[AI_NETWORK_IN_NUM] = {
 data_in_1
 };
 #else
 ai_i8* data_ins[AI_NETWORK_IN_NUM] = {
 NULL
 };
 #endif

 #if !defined(AI_NETWORK_OUTPUTS_IN_ACTIVATIONS)
 AI_ALIGNED(4) ai_i8 data_out_1[AI_NETWORK_OUT_1_SIZE_BYTES];
 ai_i8* data_outs[AI_NETWORK_OUT_NUM] = {
 data_out_1
 };
 #else
 ai_i8* data_outs[AI_NETWORK_OUT_NUM] = {
 NULL
 };
 #endif

 /* Activations buffers -------------------------------------------------------*/

 AI_ALIGNED(32)
 static uint8_t pool0[AI_NETWORK_DATA_ACTIVATION_1_SIZE];

 ai_handle data_activations0[] = {pool0};

 /* AI objects ----------------------------------------------------------------*/

 static ai_handle network = AI_HANDLE_NULL;

 AI_ALIGNED(32)
 static ai_u8 activations[AI_NETWORK_DATA_ACTIVATIONS_SIZE];

 /* Array to store the data of the input tensor */
 AI_ALIGNED(32)
 static ai_float in_data[AI_NETWORK_IN_1_SIZE];
 /* or static ai_u8 in_data[AI_NETWORK_IN_1_SIZE_BYTES]; */

 /* c-array to store the data of the output tensor */
 AI_ALIGNED(32)
 static ai_float out_data[AI_NETWORK_OUT_1_SIZE];
 /* static ai_u8 out_data[AI_NETWORK_OUT_1_SIZE_BYTES]; */

 /* Array of pointer to manage the model's input/output tensors */
 static ai_buffer* ai_input;
 static ai_buffer* ai_output;

 static void ai_log_err(const ai_error err, const char *fct)
 {
   /* USER CODE BEGIN log */
   if (fct)
     printf("TEMPLATE - Error (%s) - type=0x%02x code=0x%02x\r\n", fct,
         err.type, err.code);
   else
     printf("TEMPLATE - Error - type=0x%02x code=0x%02x\r\n", err.type, err.code);

   do {} while (1);
   /* USER CODE END log */
 }

 static int ai_boostrap(ai_handle *act_addr)
 {
   ai_error err;

   /* Create and initialize an instance of the model */
   err = ai_network_create_and_init(&network, act_addr, NULL);
   if (err.type != AI_ERROR_NONE) {
     ai_log_err(err, "ai_network_create_and_init");
     return -1;
   }

   ai_input = ai_network_inputs_get(network, NULL);
   ai_output = ai_network_outputs_get(network, NULL);

 #if defined(AI_NETWORK_INPUTS_IN_ACTIVATIONS)
   /*  In the case where "--allocate-inputs" option is used, memory buffer can be
    *  used from the activations buffer. This is not mandatory.
    */
   for (int idx=0; idx < AI_NETWORK_IN_NUM; idx++) {
   data_ins[idx] = ai_input[idx].data;
   }
 #else
   for (int idx=0; idx < AI_NETWORK_IN_NUM; idx++) {
     ai_input[idx].data = data_ins[idx];
   }
 #endif

 #if defined(AI_NETWORK_OUTPUTS_IN_ACTIVATIONS)
   /*  In the case where "--allocate-outputs" option is used, memory buffer can be
    *  used from the activations buffer. This is no mandatory.
    */
   for (int idx=0; idx < AI_NETWORK_OUT_NUM; idx++) {
   data_outs[idx] = ai_output[idx].data;
   }
 #else
   for (int idx=0; idx < AI_NETWORK_OUT_NUM; idx++) {
   ai_output[idx].data = data_outs[idx];
   }
 #endif

   return 0;
 }

 static int ai_run(void)
 {
   ai_i32 batch;

   batch = ai_network_run(network, ai_input, ai_output);
   if (batch != 1) {
     ai_log_err(ai_network_get_error(network),
         "ai_network_run");
     return -1;
   }

   return 0;
 }

 /* USER CODE BEGIN 2 */
 uint32_t getCurrentMicros(void)
 {
   LL_SYSTICK_IsActiveCounterFlag();
   uint32_t m = HAL_GetTick();
   const uint32_t tms = SysTick->LOAD + 1;
   __IO uint32_t u = tms - SysTick->VAL;
   if (LL_SYSTICK_IsActiveCounterFlag())
   {
     m = HAL_GetTick();
     u = tms - SysTick->VAL;
   }
   return (m*1000+(u*1000)/tms);
 }



 int acquire_and_process_data(ai_i8* data[])
 {
   /* fill the inputs of the c-model
   for (int idx=0; idx < AI_NETWORK_IN_NUM; idx++ )
   {
       data[idx] = ....
   }

   */
   return 0;
 }

 int post_process(ai_i8* data[])
 {
   /* process the predictions
   for (int idx=0; idx < AI_NETWORK_OUT_NUM; idx++ )
   {
       data[idx] = ....
   }

   */
   return 0;
 }
 /* USER CODE END 2 */

 /* Entry points --------------------------------------------------------------*/
 int aiInit(void) {
   ai_error err;
   /* Create and initialize the c-model */
   const ai_handle acts[] = { activations };
   err = ai_network_create_and_init(&network, acts, NULL);
   if (err.type != AI_ERROR_NONE)
   {
     return -1;
   };
   /* Reteive pointers to the model's input/output tensors */
   ai_input = ai_network_inputs_get(network, NULL);
   ai_output = ai_network_outputs_get(network, NULL);

   return 0;
 }

 /*
  * Run inference
  */
 int aiRun(const void *in_data, void *out_data) {
   ai_i32 n_batch;
   ai_error err;

   /* 1 - Update IO handlers with the data payload */
   ai_input[0].data = AI_HANDLE_PTR(in_data);
   ai_output[0].data = AI_HANDLE_PTR(out_data);

   /* 2 - Perform the inference */
   n_batch = ai_network_run(network, &ai_input[0], &ai_output[0]);
   if (n_batch != 1) {
       err = ai_network_get_error(network);
       return err.code;
   };

   return 0;
 }
 void MX_X_CUBE_AI_Init(void)
 {
     /* USER CODE BEGIN 5 */
     /* Activation/working buffer is allocated as a static memory chunk
      * (bss section) */
     AI_ALIGNED(4)
     static ai_u8 activations[AI_NETWORK_DATA_ACTIVATIONS_SIZE];

     aiInit();
     /* USER CODE END 5 */
 }

 int MX_X_CUBE_AI_Process() {
     int res;
     AI_ALIGNED(4) static ai_i8 in_data[AI_NETWORK_IN_1_SIZE_BYTES];
     AI_ALIGNED(4) static ai_i8 out_data[AI_NETWORK_OUT_1_SIZE_BYTES];

     /* Perform inference */
     res = aiRun(in_data, out_data);
     if (res) {
         return;
     }

     /* Process output to determine predicted class */
     int predicted_class = 0;
     float max_value = -1.0f;

     for (int i = 0; i < AI_NETWORK_OUT_1_SIZE; i++) {

         ai_i8 raw_value = ((ai_i8 *)out_data)[i];

         ai_buffer_format fmt_ = AI_BUFFER_FORMAT(&ai_output[0]);
         uint32_t type_ = AI_BUFFER_FMT_GET_TYPE(fmt_);

         // Experiment with different scaling approaches
         // Option 1: Assume STM32Cube.AI has fixed-point scaling
         ai_i16 N_ = AI_BUFFER_FMT_GET_FBITS(fmt_); // Fractional bits
         float value = (float)raw_value / (1 << N_); // Convert INT8 -> FLOAT

         // Option 2: Normalize INT8 [-128, 127] to [-1, 1]
//          float value = raw_value / 128.0f;

         // Option 3: Normalize to [0, 1]
//          float value = (raw_value + 128.0f) / 255.0f;

//         float value = ((ai_float *)out_data)[i];  // Convert to float
         if (value > max_value) {
             max_value = value;
             predicted_class = i;
         }
     }

     int sum_exp = 0;
     for (int i = 0; i < AI_NETWORK_OUT_1_SIZE; i++) {
         if (out_data[i] > 0)
    	 	 sum_exp += exp(((ai_float *)out_data)[i]);
     }

     /* Send predicted class via UART */
    return predicted_class;
 }

 #ifdef __cplusplus
 }
 #endif














 /* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "app_x-cube-ai.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t input[28 * 28];  // A single array of 784 floats
float result[10]={0};
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
#include "usart.h"
#include <stdio.h>


int main(void)
{
	/* USER CODE BEGIN 1 */
	float input_tmp;
	int status=0;

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
  MX_CRC_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_X_CUBE_AI_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      printf("Ready for input\r\n");

      // Receive 28x28 grayscale image (784 floats)
      HAL_UART_Receive(&huart2, (uint8_t*)input, 28 * 28 * sizeof(uint8_t), HAL_MAX_DELAY);
      printf("Image received\r\n");

      // Run inference
      int prediction = MX_X_CUBE_AI_Process();

//      int prediction = 1;

      char predicted[10];
      sprintf(predicted, "%d\r\n", prediction);
      // Send prediction result
      HAL_UART_Transmit(&huart2, (uint8_t*)predicted, sizeof(predicted), HAL_MAX_DELAY);

      HAL_Delay(500); // Small delay before next input
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

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
