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
#define NOTIFY_BIT_500MS    (1UL << 1)
#define NOTIFY_BIT_1000MS   (1UL << 2)

static void s_timer_callback(TimerHandle_t xTimer)
{
  uint32_t ulbitMask = (uint32_t)pvTimerGetTimerID(xTimer);
  if (xTimerEventTaskHandle != NULL) {
    xTaskNotify(xTimerEventTaskHandle, ulbitMask, eSetBits);
  }
  
}


void syster_timer_init(void)
{
  TimerHandle_t xtimer500ms = xTimerCreate("timer500ms",pdMS_TO_TICKS(500),pdTRUE,(void*)NOTIFY_BIT_500MS,s_timer_callback);
  TimerHandle_t xtimer1000ms = xTimerCreate("timer1000ms",pdMS_TO_TICKS(1000),pdTRUE,(void*)NOTIFY_BIT_1000MS,s_timer_callback);

  if (xtimer1000ms != NULL || xtimer500ms != NULL) {
    xTimerStart(xtimer500ms, 0);
    xTimerStart(xtimer1000ms, 0);
  }
}

//解决soft_timer下不易使用pq_push的问题
//tasknotify 比 消息队列开销更下
void task_timer_event(void *argument)
{
  struct event_t evt;
  uint32_t NotifyValue;

  xTimerEventTaskHandle = xTaskGetCurrentTaskHandle();

  for(;;)
  {
    if (xTaskNotifyWait(0x00, 0xFFFFFFFF, &NotifyValue, portMAX_DELAY) == pdTRUE) {

      if (NotifyValue & NOTIFY_BIT_10MS ) {
        evt.id = EVT_TIMER_10MS;
        evt.prio = EVT_PRIO_LOW;
        pq_push(g_EventQueue, &evt, portMAX_DELAY);
      }
      if (NotifyValue & NOTIFY_BIT_500MS ) {
        evt.id = EVT_TIMER_500MS;
        evt.prio = EVT_PRIO_HIGH;
        pq_push(g_EventQueue, &evt, portMAX_DELAY);
      }
      if (NotifyValue & NOTIFY_BIT_1000MS) {
        evt.id = EVT_TIMER_1000MS;
        evt.prio = EVT_PRIO_LOW;
        pq_push(g_EventQueue, &evt, portMAX_DELAY);
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


