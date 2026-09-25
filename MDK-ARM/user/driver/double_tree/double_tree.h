#ifndef __DOUBLE_TREE_H
#define __DOUBLE_TREE_H

#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "event.h"

#ifndef PQ_MAX_CAPACITY
#define PQ_MAX_CAPACITY 64
#endif

typedef struct PrioQueue_t PrioQueue_t;

BaseType_t pq_push(const struct event_t* event,TickType_t timeout);

BaseType_t pq_push_simple(enum event_id_e id,
  enum event_prio_e prio,TickType_t timeout);
  
BaseType_t pq_pushfromIsr(const struct event_t *event, BaseType_t *pxHigherprioTaskWoken);

BaseType_t pq_pop(struct event_t *out_event, TickType_t timeout);

#endif

