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

PrioQueue_t *g_EventQueue;
extern osThreadId_t task_10ms_highHandle;
extern QueueHandle_t      uart_tx_queue ;
extern QueueHandle_t      uart_rx_queue ;
extern SemaphoreHandle_t  dispatch_semap;
extern QueueHandle_t      ano_tx_queue;
extern QueueHandle_t event_queue[EVT_PRIO_MAX];
/////////////////////////////   以下会是测试代码                  ////////////////////////////
//分发的本质就是直接在这个线程里面执行对应的handler
//如果是要触发别的线程的话，是不是可以引入非阻塞机制呢？
//保证这个是进行事件分配的线程，这个线程可以进一步分配事件给其他线程执行
// #define EVT_MENU_REFRESH  ((enum event_id_e)300)



// static void led_on_event(enum event_id_e id,uint32_t param,void* user)
// {
//   ARG_UNUSED(param);
//   ARG_UNUSED(user);
//   if (id == EVT_KEY_PRESSED) {
//     HAL_UART_Transmit(&huart1, "led_on\r\n", 8, HAL_MAX_DELAY);
//   }
// }

// static void key_pressed_event(enum event_id_e id,uint32_t param,void* user)
// {
//   ARG_UNUSED(param);
//   ARG_UNUSED(user);
//   // if (id == EVT_KEY_PRESSED ) {
//     HAL_UART_Transmit(&huart1, "key_pressed\r\n", 13, HAL_MAX_DELAY);
//   // }
// }


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


// static void callback_500ms_low(enum event_id_e id,uint32_t param,void* user)
// {
//   HAL_UART_Transmit(&huart1, "500ms_low\r\n",11,HAL_MAX_DELAY);
// }

// static void callback_1000ms_high(enum event_id_e id,uint32_t param,void* user)
// {
//   HAL_UART_Transmit(&huart1, "1000ms_high\r\n",13,HAL_MAX_DELAY);
// }

int test_callback(uint8_t* data,uint32_t len32,void* user_data)
{
    HAL_UART_Transmit(&huart1, data, len32, HAL_MAX_DELAY);
    event_publish_ay(EVT_KEY_PRESSED,0,1);
    return 0;
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


void ano_callback(uint16_t base , uint16_t frame)
{

  uint8_t ano_device[2] = {base,frame};
  HAL_UART_Transmit(&huart1, ano_device, 2,HAL_MAX_DELAY);
}



static void ano_com_event(enum event_id_e id,uint32_t param,void* user)
{
  xQueueSend(ano_tx_queue,(struct ano_event_t*)user,0);
}



void task_10ms_low_fun(void *argument)
{
    struct ano_event_t ano_event;
    for (;;)
    {
        xQueueReceive(ano_tx_queue, &ano_event, portMAX_DELAY);
            // ano_send_data(ano_event.base,ano_event.id);
        uint8_t data[2] = {ano_event.ano_base, ano_event.ano_id};
        HAL_UART_Transmit(&huart1, data, 2, HAL_MAX_DELAY);
    }
}




//这个队列内部应该是有 *号解引用指针的，将数据copy进队列里面
void task_event(void *argument)
{
  /* USER CODE BEGIN task_10ms_high_fun */
  // struct ano_event_t event;
  driver_init_all();
  uart_register_callback(g_uart_computer, test_callback,NULL);
  
  struct ano_event_t base1_1 = {.ano_id = 1,.ano_base = 1};
  struct ano_event_t base2_2 = {.ano_base = 2,.ano_id = 2};
  struct ano_event_t base2_3 = {.ano_base = 2,.ano_id = 3};

  event_subscribe(EVT_TIMER_10MS,ano_com_event,&base2_3,3);
  event_subscribe(EVT_TIMER_10MS,ano_com_event,&base2_2,2);
  event_subscribe(EVT_TIMER_10MS,ano_com_event,&base1_1,1);
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