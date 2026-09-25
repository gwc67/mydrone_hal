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
extern QueueHandle_t      ano_tx_queue;

TaskHandle_t xTimerEventTaskHandle = NULL;

#define NOTIFY_BIT_10MS     (1UL << 0)
#define NOTIFY_BIT_20MS     (1UL << 1)
#define NOTIFY_BIT_100MS    (1UL << 2)
#define NOTIFY_BIT_500MS    (1UL << 3)
#define NOTIFY_BIT_1000MS   (1UL << 4)

static void s_timer_callback(TimerHandle_t xTimer)
{
  uint32_t ulbitMask = (uint32_t)pvTimerGetTimerID(xTimer);
  if (xTimerEventTaskHandle != NULL) {
    xTaskNotify(xTimerEventTaskHandle, ulbitMask, eSetBits);
  }
  
}

void syster_timer_init(void)
{
  TimerHandle_t xtimer10ms = xTimerCreate("timer10ms",pdMS_TO_TICKS(10),pdTRUE,(void*)NOTIFY_BIT_10MS,s_timer_callback);
  TimerHandle_t xtimer20ms = xTimerCreate("timer20ms",pdMS_TO_TICKS(20),pdTRUE,(void*)NOTIFY_BIT_20MS,s_timer_callback);
  TimerHandle_t xtimer100ms = xTimerCreate("timer100ms",pdMS_TO_TICKS(100),pdTRUE,(void*)NOTIFY_BIT_100MS,s_timer_callback);
  TimerHandle_t xtimer500ms = xTimerCreate("timer500ms",pdMS_TO_TICKS(500),pdTRUE,(void*)NOTIFY_BIT_500MS,s_timer_callback);
  TimerHandle_t xtimer1000ms = xTimerCreate("timer1000ms",pdMS_TO_TICKS(1000),pdTRUE,(void*)NOTIFY_BIT_1000MS,s_timer_callback);

  if (xtimer1000ms != NULL || xtimer500ms != NULL) {
    xTimerStart(xtimer10ms, 0);
    xTimerStart(xtimer20ms, 0);
    xTimerStart(xtimer100ms, 0);
    xTimerStart(xtimer500ms, 0);
    xTimerStart(xtimer1000ms, 0);
  }
}

//解决soft_timer下不易使用pq_push的问题
//tasknotify 比 消息队列开销更下
void task_timer_event(void *argument)
{
  uint32_t NotifyValue;
  xTimerEventTaskHandle = xTaskGetCurrentTaskHandle();

  for(;;)
  {
    if (xTaskNotifyWait(0x00, 0xFFFFFFFF, &NotifyValue, portMAX_DELAY) == pdTRUE) {
      if (NotifyValue & NOTIFY_BIT_10MS ) {
        pq_push_simple(g_EventQueue,EVT_TIMER_10MS,EVT_PRIO_HIGH,portMAX_DELAY);
      }
      else if (NotifyValue & NOTIFY_BIT_20MS ) {
        pq_push_simple(g_EventQueue,EVT_TIMER_20MS,EVT_PRIO_NORMAL,portMAX_DELAY);
      }
      else if (NotifyValue & NOTIFY_BIT_100MS ) {
        pq_push_simple(g_EventQueue,EVT_TIMER_100MS,EVT_PRIO_NORMAL,portMAX_DELAY);
      }
      else if (NotifyValue & NOTIFY_BIT_500MS ) {
        pq_push_simple(g_EventQueue,EVT_TIMER_500MS,EVT_PRIO_LOW,portMAX_DELAY);
      }
      else if (NotifyValue & NOTIFY_BIT_1000MS) {
        pq_push_simple(g_EventQueue,EVT_TIMER_1000MS,EVT_PRIO_LOW,portMAX_DELAY);
      }
    }
  }

}
// ######################################################

void task_rx(void *argument)
{
    struct uart_event_t rx_event;
    BaseType_t ret;

    for (;;)
    {
        ret = xQueueReceive(uart_rx_queue, &rx_event, portMAX_DELAY);
        if (rx_event.type_e == UART_EVENT_RX_DATA && ret == pdTRUE)
        {
            uart_rx_analyze(rx_event.base);
        }
    }
}

void task_tx(void *argument)
{

    struct uart_event_t tx_event;
    BaseType_t ret;

    for (;;)
    {
        ret = xQueueReceive(uart_tx_queue, &tx_event, portMAX_DELAY);
        if (ret == pdTRUE)
        {
            uart_tx_callback(tx_event.base, tx_event.type_e);
        }
    }
}

void task_ano_send(void *argument)
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

  driver_init_all();
  g_EventQueue = pq_create();
  struct event_t receiveEvent;

  for(;;)
  {
    if (pq_pop(g_EventQueue, &receiveEvent, portMAX_DELAY)) {
      dispatch_event(&receiveEvent);
    }
  }
}


