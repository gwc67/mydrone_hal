#include "event.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "driver_registry.h"

struct sub_item_t {
    enum event_id_e id;
    event_handler_t handler;
    void *user;
    uint8_t used;
    uint8_t priority; // 优先级，数值越小，优先级越高 解决同一个事件下，不同订阅者的优先级
};

#define SUB_MAX 32 
#define SUB_PRIO_MAX 100
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

void event_subscribe(enum event_id_e id, event_handler_t handler, void *user, uint8_t priority)
{

    if (priority > SUB_PRIO_MAX)
    {
        return;
    }

    uint16_t i;
    for (i = 0; i < SUB_MAX; i++)
    {
        if (s_subs[i].used == 0)
        {
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


void dispatch_event(struct event_t *e)
{
    for (int16_t prior = 0; prior < SUB_PRIO_MAX; prior++)
    {
        for (uint16_t i = 0; i < SUB_MAX; i++)
        {
            if (s_subs[i].used && s_subs[i].id == e->id && s_subs[i].priority == prior)
            {
                s_subs[i].handler(e->id, e->param, s_subs[i].user);
            }
        }
    }
}

// void event_publish_ay(enum event_id_e id, uint32_t param, enum event_prio_e prior)
// {
//     struct event_t e = {.id = id, .param = param};
//     xQueueSend(event_queue[prior], &e, 0);
//     xSemaphoreGive(dispatch_semap);
// }

// void event_publish_ay_isr(enum event_id_e id,uint32_t param,enum event_prio_e prior)
// {
//     struct event_t e = {.id = id,.param = param};
//     BaseType_t xtaskwoken = pdFALSE;
//     xQueueSendToBackFromISR(event_queue[prior], &e, &xtaskwoken);
//     xSemaphoreGiveFromISR(dispatch_semap,&xtaskwoken);
//     portYIELD_FROM_ISR(xtaskwoken);
// }

