/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "semphr.h"
#include "D:\Downloads\stm32project\mydrone_hal\MDK-ARM\user\driver\uart\uart_base.h"
#include "D:\Downloads\stm32project\mydrone_hal\MDK-ARM\user\user_control\scheuler\scheuler.h"
QueueHandle_t uart_tx_queue = NULL;
QueueHandle_t uart_rx_queue = NULL;
SemaphoreHandle_t xevent_dispatch = NULL;
QueueHandle_t ano_tx_queue = NULL;

extern void syster_timer_init(void);


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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for task_1ms */
osThreadId_t task_1msHandle;
const osThreadAttr_t task_1ms_attributes = {
  .name = "task_1ms",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for task_tx_event */
osThreadId_t task_tx_eventHandle;
const osThreadAttr_t task_tx_event_attributes = {
  .name = "task_tx_event",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for task_rx_event */
osThreadId_t task_rx_eventHandle;
const osThreadAttr_t task_rx_event_attributes = {
  .name = "task_rx_event",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for task_10ms_low */
osThreadId_t task_10ms_lowHandle;
const osThreadAttr_t task_10ms_low_attributes = {
  .name = "task_10ms_low",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task_1ms_dt */
osThreadId_t task_1ms_dtHandle;
const osThreadAttr_t task_1ms_dt_attributes = {
  .name = "task_1ms_dt",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for task_100ms */
osThreadId_t task_100msHandle;
const osThreadAttr_t task_100ms_attributes = {
  .name = "task_100ms",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task_10ms_high */
osThreadId_t task_10ms_highHandle;
const osThreadAttr_t task_10ms_high_attributes = {
  .name = "task_10ms_high",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal2,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void task_1ms_fun(void *argument);
void task_tx(void *argument);
void task_rx(void *argument);
void task_10ms_low_fun(void *argument);
void task_1ms_dt_fun(void *argument);
void task_100ms_fun(void *argument);
void task_10ms_high_fun(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}
/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of task_1ms */
  task_1msHandle = osThreadNew(task_1ms_fun, NULL, &task_1ms_attributes);

  /* creation of task_tx_event */
  task_tx_eventHandle = osThreadNew(task_tx, NULL, &task_tx_event_attributes);

  /* creation of task_rx_event */
  task_rx_eventHandle = osThreadNew(task_rx, NULL, &task_rx_event_attributes);

  /* creation of task_10ms_low */
  task_10ms_lowHandle = osThreadNew(task_10ms_low_fun, NULL, &task_10ms_low_attributes);

  /* creation of task_1ms_dt */
  task_1ms_dtHandle = osThreadNew(task_1ms_dt_fun, NULL, &task_1ms_dt_attributes);

  /* creation of task_100ms */
  task_100msHandle = osThreadNew(task_100ms_fun, NULL, &task_100ms_attributes);

  /* creation of task_10ms_high */
  task_10ms_highHandle = osThreadNew(task_10ms_high_fun, NULL, &task_10ms_high_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
    uart_tx_queue = xQueueCreate(10, sizeof(struct uart_event_t));
    uart_rx_queue = xQueueCreate(10, sizeof(struct uart_event_t));
    ano_tx_queue = xQueueCreate(10,sizeof(struct ano_event_t));
    xevent_dispatch = xSemaphoreCreateBinary();
  syster_timer_init();
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_task_1ms_fun */
/**
  * @brief  Function implementing the task_1ms thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_task_1ms_fun */
__weak void task_1ms_fun(void *argument)
{
  /* USER CODE BEGIN task_1ms_fun */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_1ms_fun */
}

/* USER CODE BEGIN Header_task_tx */
/**
* @brief Function implementing the task_tx_event thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_tx */
__weak void task_tx(void *argument)
{
  /* USER CODE BEGIN task_tx */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_tx */
}

/* USER CODE BEGIN Header_task_rx */
/**
* @brief Function implementing the task_rx_event thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_rx */
__weak void task_rx(void *argument)
{
  /* USER CODE BEGIN task_rx */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_rx */
}

/* USER CODE BEGIN Header_task_10ms_low_fun */
/**
* @brief Function implementing the task_10ms_low thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_10ms_low_fun */
__weak void task_10ms_low_fun(void *argument)
{
  /* USER CODE BEGIN task_10ms_low_fun */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_10ms_low_fun */
}

/* USER CODE BEGIN Header_task_1ms_dt_fun */
/**
* @brief Function implementing the task_1ms_dt thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_1ms_dt_fun */
__weak void task_1ms_dt_fun(void *argument)
{
  /* USER CODE BEGIN task_1ms_dt_fun */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_1ms_dt_fun */
}

/* USER CODE BEGIN Header_task_100ms_fun */
/**
* @brief Function implementing the task_100ms thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_100ms_fun */
__weak void task_100ms_fun(void *argument)
{
  /* USER CODE BEGIN task_100ms_fun */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_100ms_fun */
}

/* USER CODE BEGIN Header_task_10ms_high_fun */
/**
* @brief Function implementing the task_10ms_high thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_10ms_high_fun */
__weak void task_10ms_high_fun(void *argument)
{
  /* USER CODE BEGIN task_10ms_high_fun */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_10ms_high_fun */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

