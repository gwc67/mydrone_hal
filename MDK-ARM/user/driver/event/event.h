#ifndef __EVENT_H
#define __EVENT_H

#include "main.h"
#define  HIGH_PRIO_QUEUE_SIZE 16
#define  NORMAL_PRIO_QUEUE_SIZE 32



enum event_id_e
{
    EVT_NONE,
    EVT_TIMER_10MS,
    EVT_TIMER_500MS,
    EVT_TIMER_1000MS,
};

// 定义优先级枚举，数值越大，优先级越高
enum event_prio_e
{
    EVT_PRIO_LOW,
    EVT_PRIO_NORMAL,
    EVT_PRIO_HIGH,
    EVT_PRIO_MAX  // 用于记录队列总数
};

struct event_t {
  enum event_id_e id;
  enum event_prio_e prio;
  uint32_t param;
  uint32_t seq;
};


typedef void (*event_handler_t)(enum event_id_e id, uint32_t param, void *user);

void event_publish_sy(enum event_id_e id,uint32_t param);

// void event_publish_ay(enum event_id_e id,uint32_t param,uint8_t prior);

// void event_publish_ay_isr(enum event_id_e id,uint32_t param,enum event_prio_e prior);

void event_subscribe(enum event_id_e id,event_handler_t handler,void *user,uint8_t priority);

void dispatch_event(struct event_t *e);

// #define EVT_KEY_PRESSED   ((enum event_id_e)100)
// #define EVT_LED_ON        ((enum event_id_e)200)
// #define EVT_TIMER_10MS    ((enum event_id_e)300)
// #define EVT_TIMER_500MS    ((enum event_id_e)301)

#endif
