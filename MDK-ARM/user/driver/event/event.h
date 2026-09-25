#ifndef __EVENT_H
#define __EVENT_H

#include "main.h"
#define  HIGH_PRIO_QUEUE_SIZE 16
#define  NORMAL_PRIO_QUEUE_SIZE 32

typedef int16_t sub_handler_t  ;

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

sub_handler_t event_subscribe(enum event_id_e id, event_handler_t handler, void *user, uint8_t priority);

void dispatch_event(struct event_t *e);

int event_desubscribe(sub_handler_t handler);


#endif
