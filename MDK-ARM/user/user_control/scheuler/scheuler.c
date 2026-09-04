#include "scheuler.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "usart.h"
#include "cmsis_os2.h"
#include "ring_buffer.h"
#include "uarts.h"
#include "driver_registry.h"
extern SemaphoreHandle_t  test;
uint8_t rx_buffer_puc[20];
volatile bool uart_tx_is_busy = false;

uint8_t tx_ring_mem[1024];


extern QueueHandle_t uart_tx_queue ;
extern QueueHandle_t uart_rx_queue ;

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
    struct uart_base_t* me = s_uart_get_base(&huart);
    uart_isr(me, Size);
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    BaseType_t xtaskwoken = pdFALSE;
    struct uart_event_t done_event;
    if (huart == &huart1) {
        done_event.type_e = UART_EVENT_TX_DONE;
        done_event.data_ptr = NULL;
        done_event.size = 0;
        xQueueSendFromISR(uart_tx_queue, &done_event, &xtaskwoken);
        portYIELD_FROM_ISR(xtaskwoken);
    }
}


//之后封装的话，应该是调用对应经过封装过的uart_transmit_id 了
//那就串口发送使用一个队列，其他不需要句柄的就使用另外一个队列，通过id_e 进行区分
void task_100ms_fun(void *argument)
{

   struct uart_event_t rx_event;
    BaseType_t ret;
    static uint8_t tx_ptr[100];
  for(;;)
  {

    ret = xQueueReceive(uart_rx_queue, &rx_event, portMAX_DELAY);

    if (ret == pdTRUE && rx_event.type_e == UART_EVENT_TX_REQ) {
        if (!uart_tx_is_busy) {
            uint32_t tx_len = ring_buf_get(&tx_test_st, tx_ptr, sizeof(tx_ptr));
            if (HAL_UART_Transmit_IT(&huart1, tx_ptr, tx_len)) {
                uart_tx_is_busy = true;
            }
        }
    }
    else if (rx_event.type_e == UART_EVENT_TX_DONE) {

        if (ring_buf_is_empty(&tx_test_st)) {
            uart_tx_is_busy = false;
        }
        else {
            
            uint32_t tx_len = ring_buf_get(&tx_test_st, tx_ptr, 100);
            HAL_UART_Transmit_IT(&huart1,tx_ptr,tx_len);
        }
        
        HAL_UART_Transmit(&huart1,"transmit_tx_done\r\n",18,HAL_MAX_DELAY);
    }
    else if (rx_event.type_e == UART_EVENT_RX_DATA) {
        // uint8_t buffer_puc[30];
        // uint16_t len = ring_buf_get(&rx_test_st,buffer_puc ,30);
        // if (len > 0) {
            HAL_UART_Transmit(&huart1,"data_rx\r\n",9,HAL_MAX_DELAY);
        // }
        xSemaphoreGive(test);
    }
  }
  /* USER CODE END task_100ms_fun */
}

void task_1ms_fun(void *argument)
{
  /* USER CODE BEGIN task_1ms_fun */
  /* Infinite loop */
  for(;;)
  {
    if (xSemaphoreTake(test, portMAX_DELAY) == pdTRUE) {
        HAL_UART_Transmit(&huart1, "parse_command",13 ,HAL_MAX_DELAY);
    }
  }
  /* USER CODE END task_1ms_fun */
}

uint8_t rx_ring_buffer_puc[100];
void task_10ms_low_fun(void *argument)
{
  /* USER CODE BEGIN task_10ms_low_fun */
//   struct uart_event_t tx_req;
//   static  uint8_t  my_data[] = "hello freertos\r\n";
//   ring_buf_init(&rx_test_st, sizeof(rx_ring_buffer_puc), rx_ring_buffer_puc);
//   ring_buf_init(&tx_test_st, sizeof(tx_ring_mem),tx_ring_mem);
//   HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_buffer_puc, 20);
    driver_init_all();
  /* Infinite loop */
  for(;;)
  {

    // tx_req.type_e = uart_tx_is_busy ? UART_EVENT_TX_BUSY : UART_EVENT_TX_REQ;

    // ring_buf_put(&tx_test_st,my_data, strlen(my_data));
    
    // tx_req.type_e = UART_EVENT_TX_REQ;
    // tx_req.data_ptr = my_data;
    // tx_req.size = sizeof(my_data) - 1;
    // if(xQueueSend(uart_tx_queue, &tx_req, pdMS_TO_TICKS(10)));
    // {

    // }
    osDelay(1000);
  }
  /* USER CODE END task_10ms_low_fun */
}