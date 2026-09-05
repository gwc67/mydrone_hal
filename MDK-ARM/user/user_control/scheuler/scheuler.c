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

extern QueueHandle_t uart_tx_queue ;
extern QueueHandle_t uart_rx_queue ;
extern SemaphoreHandle_t xevent_dispatch;

//可以再额外搞一个定时事件队列，到对应的时间执行相应的协议TX函数，将值放到底层ring_buf里面，这个事件队列，将传递 frame_id 和 ano_device的设备

//然后对于一些没有sheng'me




/////////////////////////////   以下会是测试代码                  ////////////////////////////

#define  ASYNC_QUEUE_SIZE 32
#define SUB_MAX 32 
#define EVT_KEY_PRESSED   ((event_id_t)100)
#define EVT_LED_ON        ((event_id_t)200)
#define EVT_TIMER_10MS    ((event_id_t)300)




typedef uint16_t event_id_t;

struct event_t {
  event_id_t id;
  uint32_t param;
};
typedef void (*event_handler_t)(event_id_t id,uint32_t param,void *user);

static struct event_t async_queue[ASYNC_QUEUE_SIZE];
static volatile uint16_t q_head = 0;
static volatile uint16_t q_tail = 0;
struct sub_item_t {
  event_id_t id;
  event_handler_t handler;
  void *user;
  uint8_t used;
};

static struct sub_item_t s_subs[SUB_MAX];
static bool event_queue_push(event_id_t id,uint32_t param)
{
  uint16_t next_head = (q_head + 1) % ASYNC_QUEUE_SIZE;
  if (next_head == q_tail) {
    return false;
  }
  async_queue[q_head].id = id;
  async_queue[q_head].param = param;
  q_head = next_head;

  BaseType_t xtaskwoken = pdFALSE;
  xSemaphoreGiveFromISR(xevent_dispatch, &xtaskwoken);
  portYIELD_FROM_ISR(xtaskwoken);
  return true;
}

static bool event_queue_pop(struct event_t* e)
{
  if (q_head == q_tail) {
    return false;
  }
    *e = async_queue[q_tail];
    q_tail = (q_tail + 1) % ASYNC_QUEUE_SIZE;
    return true;
}

void event_publish_ay(event_id_t id,uint32_t param)
{
  // event_queue_push(id,param);
  struct event_t e = {.id = id,.param = param};
  event_queue_push(e.id, e.param);
}


//分发的本质就是直接在这个线程里面执行对应的handler
//如果是要触发别的线程的话，是不是可以引入非阻塞机制呢？
//保证这个是进行事件分配的线程，这个线程可以进一步分配事件给其他线程执行
void event_dispatch(void)
{
  struct event_t e;
  while (event_queue_pop(&e)) {
      for (uint16_t i = 0; i < SUB_MAX; i++) {
        if (s_subs[i].used && s_subs[i].id == e.id) {
          s_subs[i].handler(e.id,e.param,s_subs[i].user);
        }
      }
  }
}




void event_bus_init(void);
void event_subscribe(event_id_t id,event_handler_t handler,void* user);
void event_publish_sy(event_id_t id,uint32_t param);


// #define EVT_MENU_REFRESH  ((event_id_t)300)

static void led_on_event(event_id_t id,uint32_t param,void* user)
{
  ARG_UNUSED(param);
  ARG_UNUSED(user);
  if (id == EVT_KEY_PRESSED) {
    HAL_UART_Transmit(&huart1, "led_on\r\n", 8, HAL_MAX_DELAY);
  }
}

static void key_pressed_event(event_id_t id,uint32_t param,void* user)
{
  ARG_UNUSED(param);
  ARG_UNUSED(user);
  // if (id == EVT_KEY_PRESSED ) {
    HAL_UART_Transmit(&huart1, "key_pressed\r\n", 13, HAL_MAX_DELAY);
  // }
}



void led_module_init(void)
{
  event_subscribe(EVT_KEY_PRESSED, led_on_event , 0);
  event_subscribe(EVT_KEY_PRESSED, key_pressed_event , 0);
  event_subscribe(EVT_TIMER_10MS, key_pressed_event , 0);
}

void key_module_run(void)
{
    event_publish_sy(EVT_KEY_PRESSED, 0); 
}



void event_bus_init(void)
{
  uint16_t i;
  for (i = 0; i < SUB_MAX; i++) {
    s_subs[i].used = 0;
    s_subs[i].handler = 0;
    s_subs[i].user = 0;
  }
}


void event_subscribe(event_id_t id,event_handler_t handler,void *user)
{
  uint16_t i ;
  for (i = 0; i < SUB_MAX; i++) {
      if (s_subs[i].used == 0) {
        s_subs[i].id = id;
        s_subs[i].handler = handler;
        s_subs[i].used = 1;
        return;
      }
  }
  
}


void event_publish_sy(event_id_t id,uint32_t param)
{
  uint16_t i;
  for (i = 0; i < SUB_MAX; i++) {
    if ((s_subs[i].used != 0) && (s_subs[i].id == id)) {
      s_subs[i].handler(id,param,s_subs->user);
    }
  }
}


void timer_10ms_callback(TimerHandle_t xtimer)
{
   event_publish_ay(EVT_TIMER_10MS, 0);
}


void syster_timer_init(void)
{
  TimerHandle_t xtimer10ms = xTimerCreate("timer10ms",pdMS_TO_TICKS(1000),pdTRUE,NULL,timer_10ms_callback);

  if (xtimer10ms != NULL) {
    // xTimerCreate("timer10ms", pdMS_TO_TICKS(10), const UBaseType_t pdTRUE, NULL, timer_10ms_callback);
    xTimerStart(xtimer10ms, 0);
  }
}

// ######################################################

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




int test_callback(uint8_t* data,uint32_t len32,void* user_data)
{
    HAL_UART_Transmit(&huart1, data, len32, HAL_MAX_DELAY);
    event_publish_ay(EVT_KEY_PRESSED,0);
    return 0;
}

void task_10ms_low_fun(void *argument)
{
  /* USER CODE BEGIN task_10ms_low_fun */
    driver_init_all();
    uart_register_callback(g_uart_computer, test_callback,NULL);
    event_bus_init();
    led_module_init();
  /* Infinite loop */
  for(;;)
  {
    if (xSemaphoreTake(xevent_dispatch, portMAX_DELAY) == pdTRUE) {
        event_dispatch();
    }
    // key_module_run();
    // uart_transmit(g_uart_computer, "hello gwc\r\n", 11);
    // osDelay(1000);
  }
  /* USER CODE END task_10ms_low_fun */
}