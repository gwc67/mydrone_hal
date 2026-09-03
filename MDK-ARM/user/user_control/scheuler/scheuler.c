#include "scheuler.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "usart.h"
#include "cmsis_os2.h"
#include "ring_buffer.h"

SemaphoreHandle_t  test;
uint8_t rx_buffer_puc[20];
volatile bool uart_tx_is_busy = false;

uint8_t tx_ring_mem[1024];


extern QueueHandle_t uart_tx_queue ;

typedef void (*uart_event_callback_t)(UART_HandleTypeDef* uart_handle,
                                           struct uart_event_t *event,
                                           void *user_data);
struct uart_t{
    UART_HandleTypeDef* uart_handle;
    void *user_data;
    uart_event_callback_t callback;
};

void call_back_test(UART_HandleTypeDef* uart_handle,
                                           struct uart_event_t *event,
                                           void *user_data)
{
    HAL_UART_Transmit(uart_handle, "call_back", 9, HAL_MAX_DELAY);
}


struct uart_t com_uart_t = {
    .callback = call_back_test,
    .uart_handle = &huart1,
};

struct ring_buf rx_test_st;
struct ring_buf tx_test_st;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    BaseType_t xtaskwoken = pdFALSE;
    struct uart_event_t rx_event;
    if (huart == &huart1 && Size > 0) {
        ring_buf_put(&rx_test_st,rx_buffer_puc,Size);
        HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_buffer_puc, sizeof(rx_buffer_puc));
        rx_event.type_e = UART_EVENT_RX_DATA;
        rx_event.size = Size;
        xQueueSendFromISR(uart_tx_queue, &rx_event, &xtaskwoken);

        //这个作用不知到有什么用
        // 如果不加这行代码，被唤醒的任务必须等到下一个系统滴答中断（SysTick）才能被调度，这会白白浪费宝贵的时间，增加通信延迟
        portYIELD_FROM_ISR(xtaskwoken);
    }
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    BaseType_t xtaskwoken = pdFALSE;
    struct uart_event_t done_event;
    if (huart == &huart1) {
        done_event.type_e = UART_EVENT_TX_DONE;
        done_event.data_puc = NULL;
        done_event.size = 0;
        xQueueSendFromISR(uart_tx_queue, &done_event, &xtaskwoken);

        //这个作用不知到有什么用
        portYIELD_FROM_ISR(xtaskwoken);
    }
}
void task_100ms_fun(void *argument)
{

   struct uart_event_t rx_event;
    BaseType_t ret;
  for(;;)
  {

    ret = xQueueReceive(uart_tx_queue, &rx_event, portMAX_DELAY);

    if (ret == pdTRUE && rx_event.type_e == UART_EVENT_TX_REQ) {
        if (HAL_UART_Transmit_IT(&huart1,rx_event.data_puc,rx_event.size) == HAL_OK) {
            uart_tx_is_busy = true; 
        }
    }
    else if (rx_event.type_e == UART_EVENT_TX_DONE) {

        if (ring_buf_is_empty(&tx_test_st)) {
            uart_tx_is_busy = false;
        }
        else {
            static uint8_t tx_ptr[100];
            static uint32_t tx_len;
            tx_len = ring_buf_size_get(&tx_test_st);
            tx_len = ring_buf_get(&tx_test_st, tx_ptr, 100);
            HAL_UART_Transmit_IT(&huart1,tx_ptr,tx_len);
        }
        
        HAL_UART_Transmit(&huart1,"transmit_tx_done\r\n",18,HAL_MAX_DELAY);
    }
    else if (rx_event.type_e == UART_EVENT_TX_BUSY) {
        ring_buf_put(&tx_test_st, rx_event.data_puc, rx_event.size);
    }
    else if (rx_event.type_e == UART_EVENT_RX_DATA) {
        uint8_t buffer_puc[30];
        uint16_t len = ring_buf_get(&rx_test_st,buffer_puc ,30);
        if (len > 0) {
            HAL_UART_Transmit(&huart1,buffer_puc,len,HAL_MAX_DELAY);
        }
    
    }
  }
  /* USER CODE END task_100ms_fun */
}



uint8_t rx_ring_buffer_puc[100];
void task_10ms_low_fun(void *argument)
{
  /* USER CODE BEGIN task_10ms_low_fun */
  struct uart_event_t tx_req;
  static uint8_t my_data[] = "hello freertos 123456789\r\n";
  ring_buf_init(&rx_test_st, sizeof(rx_ring_buffer_puc), rx_ring_buffer_puc);
  ring_buf_init(&tx_test_st, sizeof(tx_ring_mem),tx_ring_mem);
  HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_buffer_puc, 20);
  /* Infinite loop */
  for(;;)
  {

    tx_req.type_e = uart_tx_is_busy ? UART_EVENT_TX_BUSY : UART_EVENT_TX_REQ;
    
    // tx_req.type_e = UART_EVENT_TX_REQ;
    tx_req.data_puc = my_data;
    tx_req.size = sizeof(my_data) - 1;
    if(xQueueSend(uart_tx_queue, &tx_req, pdMS_TO_TICKS(10)));
    {

    }
    osDelay(5);
  }
  /* USER CODE END task_10ms_low_fun */
}