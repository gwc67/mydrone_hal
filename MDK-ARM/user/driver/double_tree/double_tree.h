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

PrioQueue_t* pq_create(void);

BaseType_t pq_push(PrioQueue_t* pq,const struct event_t* event,TickType_t timeout);

BaseType_t pq_pushfromIsr(PrioQueue_t* pq,const struct event_t* event,BaseType_t *pxHigherPriorityTaskWoken);

BaseType_t pq_pop(PrioQueue_t *pq, struct event_t *out_event, TickType_t timeout);

#endif

