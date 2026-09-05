#include "event.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "driver_registry.h"

extern QueueHandle_t xhighprio_queue;
extern QueueHandle_t xlowprio_queue;
extern SemaphoreHandle_t  xevent_dispatch;

struct sub_item_t {
    enum event_id_e id;
    event_handler_t handler;
    void *user;
    uint8_t used;
    uint8_t priority; // 优先级，数值越小，优先级越高
};

#define SUB_MAX 32 

static struct sub_item_t s_subs[SUB_MAX];


void event_bus_init(void)
{
  uint16_t i;
  for (i = 0; i < SUB_MAX; i++) {
    s_subs[i].used = 0;
    s_subs[i].handler = 0;
    s_subs[i].user = 0;
    s_subs[i].priority = 0;
    s_subs[i].id = EVT_NONE;
}
}
DRIVER_INIT_1(event_bus_init);


void event_subscribe(enum event_id_e id,event_handler_t handler,void *user,uint8_t priority)
{
  uint16_t i ;
  for (i = 0; i < SUB_MAX; i++) {
      if (s_subs[i].used == 0) {
        s_subs[i].id = id;
        s_subs[i].handler = handler;
        s_subs[i].used = 1;
        s_subs[i].user = user;
        s_subs[i].priority = priority;
        return;
      }
  }
}


void event_publish_sy(enum event_id_e id,uint32_t param)
{
  uint16_t i;
  for (i = 0; i < SUB_MAX; i++) {
    if ((s_subs[i].used != 0) && (s_subs[i].id == id)) {
      s_subs[i].handler(id,param,s_subs->user);
    }
  }
}


void dispatch_event_to_handlers(struct event_t *e)
{
    for (uint8_t prior = 0; prior <= 254; prior++) { 
      for (uint16_t i = 0; i < SUB_MAX; i++) {
        if (s_subs[i].used && s_subs[i].id == e->id && s_subs[i].priority == prior)
         {
          s_subs[i].handler(e->id,e->param,s_subs[i].user);
        }
      }
    }
}

void event_publish_ay(enum event_id_e id,uint32_t param,uint8_t prior)
{
  struct event_t e = {.id = id,.param = param};
  BaseType_t xtaskwoken = pdFALSE;

  switch (prior) {
    case 0:
    {
      xQueueSendToBack(xhighprio_queue, &e, 0);
    }
    break;
    case 1:
    {
      //这个函数有什么作用，和xqueuesend比
      xQueueSendToBack(xlowprio_queue, &e, 0);
    }
  default:
    break;
  }

  xSemaphoreGiveFromISR(xevent_dispatch, &xtaskwoken);
  portYIELD_FROM_ISR(xtaskwoken);
}

