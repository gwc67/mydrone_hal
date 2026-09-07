#include "scheuler.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "usart.h"
#include "cmsis_os2.h"
#include "ring_buffer.h"
#include "uarts.h"
#include "driver_registry.h"
#include "mesc.h"
#include "task.h"
#include "timers.h"
#include "event.h"
#include "double_tree.h"
#include "ano_base.h"
PrioQueue_t *g_EventQueue;
extern QueueHandle_t      uart_tx_queue ;
extern QueueHandle_t      uart_rx_queue ;
extern SemaphoreHandle_t  dispatch_semap;
extern QueueHandle_t      ano_tx_queue;

void key_module_run(void)
{
    event_publish_sy(EVT_KEY_PRESSED, 0); 
}

void timer_10ms_callback(TimerHandle_t xtimer)
{
  struct event_t evt = { .id = EVT_TIMER_10MS,
  .prio = EVT_PRIO_LOW};
   pq_push(g_EventQueue,&evt,0);
   
}

void timer_500ms_callback(TimerHandle_t xtimer)
{
    struct event_t evt = {.id =EVT_TIMER_500MS,
  .prio = EVT_PRIO_LOW, };
  pq_push(g_EventQueue,&evt,0);
}

void syster_timer_init(void)
{
  TimerHandle_t xtimer10ms = xTimerCreate("timer10ms",pdMS_TO_TICKS(1000),pdTRUE,NULL,timer_10ms_callback);
  TimerHandle_t xtimer500ms = xTimerCreate("timer500ms",pdMS_TO_TICKS(1000),pdTRUE,NULL,timer_500ms_callback);

  if (xtimer10ms != NULL || xtimer500ms != NULL) {
    // xTimerCreate("timer10ms", pdMS_TO_TICKS(10), const UBaseType_t pdTRUE, NULL, timer_10ms_callback);
    xTimerStart(xtimer10ms, 0);
    xTimerStart(xtimer500ms, 0);
  }
}

// ######################################################

void task_rx(void *argument)
{
    /* USER CODE BEGIN task_user_fun */
    struct uart_event_t rx_event;
    BaseType_t ret;
    /* Infinite loop */
    for (;;)
    {
        ret = xQueueReceive(uart_rx_queue, &rx_event, portMAX_DELAY);
        if (rx_event.type_e == UART_EVENT_RX_DATA && ret == pdTRUE)
        {
            uart_rx_analyze(rx_event.base);
            HAL_UART_Transmit(&huart1, "data_rx\r\n", 9, HAL_MAX_DELAY);
        }
    }
    /* USER CODE END task_user_fun */
}

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

// void task_10ms_low_fun(void *argument)
// {
//   /* USER CODE BEGIN task_10ms_low_fun */
//     driver_init_all();
//     uart_register_callback(g_uart_computer, test_callback,NULL);
//     event_subscribe(EVT_KEY_PRESSED, led_on_event , 0,10);
//     event_subscribe(EVT_KEY_PRESSED, key_pressed_event ,0, 10);
//     event_subscribe(EVT_TIMER_500MS,callback_500ms_low,NULL,1);
//     event_subscribe(EVT_TIMER_500MS,callback_500ms_low,NULL,1);
//     event_subscribe(EVT_TIMER_10MS,callback_1000ms_high,NULL,1);

    
//     struct event_t event;
//   /* Infinite loop */
//     for (;;)
//     {
//         // 而且使用信号量的话，不是每触发一次就会事件就会进行吗？不当前只是测试，之后对于事件肯定是要

//         if (xSemaphoreTake(dispatch_semap, portMAX_DELAY) == pdTRUE)
//         {
//             for (int8_t i = EVT_PRIO_MAX - 1; i >= 0; i--)
//             {
//                 while (xQueueReceive(event_queue[i], &event, 0) == pdTRUE)
//                 {
//                     dispatch_event(&event);
//                     xSemaphoreTake(dispatch_semap, 0);
//                 }
//             }
//         }

//         // if (xQueueReceive(xhighprio_queue, &event, portMAX_DELAY)) {
//         //     dispatch_event(&event);
//         // }
//     }
//   /* USER CODE END task_10ms_low_fun */
// }

void task_10ms_low_fun(void *argument)
{
    struct ano_event_t ano_event;
    for (;;)
    {
        xQueueReceive(ano_tx_queue, &ano_event, portMAX_DELAY);
        ano_send_data(ano_event.me, ano_event.frame);
    }
}




//这个队列内部应该是有 *号解引用指针的，将数据copy进队列里面
void task_event(void *argument)
{
  /* USER CODE BEGIN task_10ms_high_fun */
  // struct ano_event_t event;
  driver_init_all();
  // struct ano_event_t base1_1 = {.ano_id = 1,.ano_base = 1};
  // struct ano_event_t base2_2 = {.ano_base = 2,.ano_id = 2};
  // struct ano_event_t base2_3 = {.ano_base = 2,.ano_id = 3};

  // event_subscribe(EVT_TIMER_10MS,ano_com_event,&base2_3,3);
  // event_subscribe(EVT_TIMER_10MS,ano_com_event,&base2_2,2);
  // event_subscribe(EVT_TIMER_10MS,ano_com_event,&base1_1,1);
  // event_subscribe(EVT_TIMER_500MS,callback_500ms_low,NULL,1);
  // event_subscribe(EVT_TIMER_500MS,callback_500ms_low,NULL,1);
  // event_subscribe(EVT_TIMER_10MS,callback_1000ms_high,NULL,1);
  g_EventQueue = pq_create();
  struct event_t receiveEvent;
  /* Infinite loop */
  for(;;)
  {
    if (pq_pop(g_EventQueue, &receiveEvent, portMAX_DELAY)) {
      dispatch_event(&receiveEvent);
    }
  }
  /* USER CODE END task_10ms_high_fun */
}