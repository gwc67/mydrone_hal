#ifndef __EVENT_H
#define __EVENT_H

#include "main.h"
#define  HIGH_PRIO_QUEUE_SIZE 16
#define  NORMAL_PRIO_QUEUE_SIZE 32



enum event_id_e
{
    EVT_NONE,
    EVT_KEY_PRESSED = 1,
    EVT_LED_ON,
    EVT_TIMER_10MS,
    EVT_TIMER_500MS,
    EVT_TIMER_1000MS,
};

struct event_t {
  enum event_id_e id;
  uint32_t param;
};



typedef void (*event_handler_t)(enum event_id_e id, uint32_t param, void *user);

void event_publish_sy(enum event_id_e id,uint32_t param);
void event_publish_ay(enum event_id_e id,uint32_t param,uint8_t prior);
void event_subscribe(enum event_id_e id,event_handler_t handler,void *user,uint8_t priority);
void dispatch_event_to_handlers(struct event_t *e);

// #define EVT_KEY_PRESSED   ((enum event_id_e)100)
// #define EVT_LED_ON        ((enum event_id_e)200)
// #define EVT_TIMER_10MS    ((enum event_id_e)300)
// #define EVT_TIMER_500MS    ((enum event_id_e)301)

#endif
