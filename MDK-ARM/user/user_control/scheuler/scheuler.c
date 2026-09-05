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

extern QueueHandle_t      uart_tx_queue ;
extern QueueHandle_t      uart_rx_queue ;
extern SemaphoreHandle_t  xevent_dispatch;
extern QueueHandle_t      ano_tx_queue;
extern QueueHandle_t xhighprio_queue;
extern QueueHandle_t xlowprio_queue;
//可以再额外搞一个定时事件队列，到对应的时间执行相应的协议TX函数，将值放到底层ring_buf里面，这个事件队列，将传递 frame_id 和 ano_device的设备

//然后对于一些没有sheng'me




/////////////////////////////   以下会是测试代码                  ////////////////////////////














//分发的本质就是直接在这个线程里面执行对应的handler
//如果是要触发别的线程的话，是不是可以引入非阻塞机制呢？
//保证这个是进行事件分配的线程，这个线程可以进一步分配事件给其他线程执行




void event_dispatch(void)
{
  struct event_t e;
  while (xQueueReceive(xhighprio_queue,&e,0 ) == pdTRUE) {
      dispatch_event_to_handlers(&e);
  }
  while (xQueueReceive(xlowprio_queue,&e,0 ) == pdTRUE) {
      dispatch_event_to_handlers(&e);
  }
}




void event_bus_init(void);
void event_subscribe(enum event_id_e id,event_handler_t handler,void *user,uint8_t priority);
void event_publish_sy(enum event_id_e id,uint32_t param);


// #define EVT_MENU_REFRESH  ((enum event_id_e)300)

static void led_on_event(enum event_id_e id,uint32_t param,void* user)
{
  ARG_UNUSED(param);
  ARG_UNUSED(user);
  if (id == EVT_KEY_PRESSED) {
    HAL_UART_Transmit(&huart1, "led_on\r\n", 8, HAL_MAX_DELAY);
  }
}

static void key_pressed_event(enum event_id_e id,uint32_t param,void* user)
{
  ARG_UNUSED(param);
  ARG_UNUSED(user);
  // if (id == EVT_KEY_PRESSED ) {
    HAL_UART_Transmit(&huart1, "key_pressed\r\n", 13, HAL_MAX_DELAY);
  // }
}

//可以对这个user进二次使用，这个user在一开始订阅的时候只需要将ano的协议句柄传过去，可是如何确定是哪个帧id呢？

#define  UNKONW_HOWDEFINE 0
static void ano_com_event(enum event_id_e id,uint32_t param,void* user)
{
  xQueueSend(ano_tx_queue,(struct ano_event_t*)user,0);
}



void led_module_init(void)
{
  event_subscribe(EVT_KEY_PRESSED, led_on_event , 0,10);
  event_subscribe(EVT_KEY_PRESSED, key_pressed_event ,0, 10);
  // event_subscribe(EVT_TIMER_10MS, key_pressed_event , 0);
}

void key_module_run(void)
{
    event_publish_sy(EVT_KEY_PRESSED, 0); 
}






void timer_10ms_callback(TimerHandle_t xtimer)
{
   event_publish_ay(EVT_TIMER_10MS, 0,0);
}

void timer_500ms_callback(TimerHandle_t xtimer)
{
  event_publish_ay(EVT_TIMER_500MS, 0, 1);
}

void syster_timer_init(void)
{
  TimerHandle_t xtimer10ms = xTimerCreate("timer10ms",pdMS_TO_TICKS(1000),pdTRUE,NULL,timer_10ms_callback);
  TimerHandle_t xtimer500ms = xTimerCreate("timer500ms",pdMS_TO_TICKS(500),pdTRUE,NULL,timer_500ms_callback);

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


static void callback_500ms_low(enum event_id_e id,uint32_t param,void* user)
{
  HAL_UART_Transmit(&huart1, "500ms_low\r\n",11,HAL_MAX_DELAY);
}

static void callback_1000ms_high(enum event_id_e id,uint32_t param,void* user)
{
  HAL_UART_Transmit(&huart1, "1000ms_high\r\n",11,HAL_MAX_DELAY);
}

int test_callback(uint8_t* data,uint32_t len32,void* user_data)
{
    HAL_UART_Transmit(&huart1, data, len32, HAL_MAX_DELAY);
    event_publish_ay(EVT_KEY_PRESSED,0,1);
    return 0;
}

void task_10ms_low_fun(void *argument)
{
  /* USER CODE BEGIN task_10ms_low_fun */
    driver_init_all();
    uart_register_callback(g_uart_computer, test_callback,NULL);
    event_bus_init();
    led_module_init();

    // struct ano_event_t base_1_1 = {
    //   .ano_base = 1,
    //   .ano_id = 1,
    // };

    // struct ano_event_t base_1_2 = {
    //   .ano_base = 1,
    //   .ano_id = 2,
    // };
    // struct ano_event_t base_2_1 = {
    //   .ano_base = 2,
    //   .ano_id = 1,
    // };
    // event_subscribe(EVT_TIMER_10MS,ano_com_event,&base_1_1,3);
    // event_subscribe(EVT_TIMER_10MS,ano_com_event,&base_1_2,2);
    // event_subscribe(EVT_TIMER_10MS,ano_com_event,&base_2_1,1);
    event_subscribe(EVT_TIMER_10MS,callback_1000ms_high,NULL,1);
    event_subscribe(EVT_TIMER_500MS,callback_500ms_low,NULL,1);
  /* Infinite loop */
  for(;;)
  {
    //而且使用信号量的话，不是每触发一次就会事件就会进行吗？不当前只是测试，之后对于事件肯定是要
    if (xSemaphoreTake(xevent_dispatch, portMAX_DELAY) == pdTRUE) {
        event_dispatch();
    }
    // key_module_run();
    // uart_transmit(g_uart_computer, "hello gwc\r\n", 11);
    // osDelay(1000);
  }
  /* USER CODE END task_10ms_low_fun */
}


void ano_callback(uint16_t base , uint16_t frame)
{

  uint8_t ano_device[2] = {base,frame};
  HAL_UART_Transmit(&huart1, ano_device, 2,HAL_MAX_DELAY);
}


void task_10ms_high_fun(void *argument)
{
  /* USER CODE BEGIN task_10ms_high_fun */
  struct ano_event_t event;
  /* Infinite loop */
  for(;;)
  {
    xQueueReceive(ano_tx_queue, &event, portMAX_DELAY);
    ano_callback(event.ano_base,event.ano_id);
  }
  /* USER CODE END task_10ms_high_fun */
}