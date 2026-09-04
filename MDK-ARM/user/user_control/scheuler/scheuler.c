#include "scheuler.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "usart.h"
#include "cmsis_os2.h"
#include "ring_buffer.h"
#include "uarts.h"
#include "driver_registry.h"
extern QueueHandle_t uart_tx_queue ;
extern QueueHandle_t uart_rx_queue ;

int test_callback(uint8_t* data,uint32_t len32,void* user_data)
{
    HAL_UART_Transmit(&huart1, data, len32, HAL_MAX_DELAY);
    return 0;
}

void task_rx(void *argument)
{
    /* USER CODE BEGIN task_user_fun */
    struct uart_event_t tx_event;
    BaseType_t ret;
    /* Infinite loop */
    for (;;)
    {
        ret = xQueueReceive(uart_rx_queue, &tx_event, portMAX_DELAY);
        if (tx_event.type_e == UART_EVENT_RX_DATA && ret == pdTRUE)
        {
            uart_rx_analyze(g_uart_computer);
            HAL_UART_Transmit(&huart1, "data_rx\r\n", 9, HAL_MAX_DELAY);
        }
    }
    /* USER CODE END task_user_fun */
}

//之后封装的话，应该是调用对应经过封装过的uart_transmit_id 了
//那就串口发送使用一个队列，其他不需要句柄的就使用另外一个队列，通过id_e 进行区分

void task_tx(void *argument)
{
    /* USER CODE BEGIN task_100ms_fun */
    struct uart_event_t tx_event;
    BaseType_t ret;
    /* Infinite loop */
    for (;;)
    {
        ret = xQueueReceive(uart_tx_queue, &tx_event, portMAX_DELAY);
        if (ret == pdTRUE)
        {
            uart_tx_callback(tx_event.base, tx_event.type_e);
        }
    }
    /* USER CODE END task_100ms_fun */
}

void task_10ms_low_fun(void *argument)
{
  /* USER CODE BEGIN task_10ms_low_fun */
    driver_init_all();
    uart_register_callback(g_uart_computer, test_callback,NULL);

  /* Infinite loop */
  for(;;)
  {
    uart_transmit(g_uart_computer, "hello gwc\r\n", 11);
    osDelay(1000);
  }
  /* USER CODE END task_10ms_low_fun */
}