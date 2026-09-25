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
#include "D:\Downloads\stm32project\mydrone_hal\MDK-ARM\user\user_control\ano\ano_base.h"
QueueHandle_t uart_tx_queue = NULL;
QueueHandle_t uart_rx_queue = NULL;
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
/* Definitions for task_1ms_name */
osThreadId_t task_1ms_nameHandle;
const osThreadAttr_t task_1ms_name_attributes = {
  .name = "task_1ms_name",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for task_tx_event */
osThreadId_t task_tx_eventHandle;
const osThreadAttr_t task_tx_event_attributes = {
  .name = "task_tx_event",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for task_rx_event */
osThreadId_t task_rx_eventHandle;
const osThreadAttr_t task_rx_event_attributes = {
  .name = "task_rx_event",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for task_ano_send_e */
osThreadId_t task_ano_send_eHandle;
const osThreadAttr_t task_ano_send_e_attributes = {
  .name = "task_ano_send_e",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal6,
};
/* Definitions for task_event_name */
osThreadId_t task_event_nameHandle;
const osThreadAttr_t task_event_name_attributes = {
  .name = "task_event_name",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal7,
};
/* Definitions for task_timer_even */
osThreadId_t task_timer_evenHandle;
const osThreadAttr_t task_timer_even_attributes = {
  .name = "task_timer_even",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal1,
};
/* Definitions for task_10ms_name */
osThreadId_t task_10ms_nameHandle;
const osThreadAttr_t task_10ms_name_attributes = {
  .name = "task_10ms_name",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal5,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void task_1ms(void *argument);
void task_tx(void *argument);
void task_rx(void *argument);
void task_ano_send(void *argument);
void task_event(void *argument);
void task_timer_event(void *argument);
void task_10ms(void *argument);

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
  /* creation of task_1ms_name */
  task_1ms_nameHandle = osThreadNew(task_1ms, NULL, &task_1ms_name_attributes);

  /* creation of task_tx_event */
  task_tx_eventHandle = osThreadNew(task_tx, NULL, &task_tx_event_attributes);

  /* creation of task_rx_event */
  task_rx_eventHandle = osThreadNew(task_rx, NULL, &task_rx_event_attributes);

  /* creation of task_ano_send_e */
  task_ano_send_eHandle = osThreadNew(task_ano_send, NULL, &task_ano_send_e_attributes);

  /* creation of task_event_name */
  task_event_nameHandle = osThreadNew(task_event, NULL, &task_event_name_attributes);

  /* creation of task_timer_even */
  task_timer_evenHandle = osThreadNew(task_timer_event, NULL, &task_timer_even_attributes);

  /* creation of task_10ms_name */
  task_10ms_nameHandle = osThreadNew(task_10ms, NULL, &task_10ms_name_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
    uart_tx_queue = xQueueCreate(30, sizeof(struct uart_event_t));
    uart_rx_queue = xQueueCreate(30, sizeof(struct uart_event_t));
    ano_tx_queue = xQueueCreate(30,sizeof(struct ano_event_t));
  syster_timer_init();
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_task_1ms */
/**
  * @brief  Function implementing the task_1ms_name thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_task_1ms */
__weak void task_1ms(void *argument)
{
  /* USER CODE BEGIN task_1ms */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_1ms */
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

/* USER CODE BEGIN Header_task_ano_send */
/**
* @brief Function implementing the task_ano_send_e thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_ano_send */
__weak void task_ano_send(void *argument)
{
  /* USER CODE BEGIN task_ano_send */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_ano_send */
}

/* USER CODE BEGIN Header_task_event */
/**
* @brief Function implementing the task_event_name thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_event */
__weak void task_event(void *argument)
{
  /* USER CODE BEGIN task_event */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_event */
}

/* USER CODE BEGIN Header_task_timer_event */
/**
* @brief Function implementing the task_timer_even thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_timer_event */
__weak void task_timer_event(void *argument)
{
  /* USER CODE BEGIN task_timer_event */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_timer_event */
}

/* USER CODE BEGIN Header_task_10ms */
/**
* @brief Function implementing the task_10ms_name thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_10ms */
__weak void task_10ms(void *argument)
{
  /* USER CODE BEGIN task_10ms */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_10ms */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

