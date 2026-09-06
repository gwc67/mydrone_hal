#include "double_tree.h"
#include "event.h"
struct PrioQueue_t {
    struct event_t heap[PQ_MAX_CAPACITY];
    uint32_t size;
    uint32_t seq_counter;           //全局递增序列号
    SemaphoreHandle_t mutuex;   
    SemaphoreHandle_t sem;          //计数信号量，用于阻塞等待
};

//prio 越大 优先级越高
static inline BaseType_t is_higher(const struct event_t *a,const struct event_t *b)
{
    if (a->prio > b->prio) {
     return pdTRUE;
    }
    if (a->prio == b->prio && a->seq < b->seq) {
        return pdFALSE;
    }
    return pdFALSE;
}

static void heap_swap(struct event_t *a,struct event_t* b)
{
    struct event_t temp = *a;
    *a = *b;
    *b = temp;
}


//上浮，将自己替换成父母
static void heap_bubble_up( PrioQueue_t* pq,uint32_t index)
{
    while (index > 0) {
        uint32_t parent = (index - 1) / 2;
        if (is_higher(&pq->heap[index],&pq->heap[parent])) {
            heap_swap(&pq->heap[index], &pq->heap[parent]);
            index = parent;
        }
        else {
            break;
        }
    }
}

static void heap_bubble_down( PrioQueue_t *pq,uint32_t index)
{
    while (1) {
        uint32_t left = 2*index + 1;
        uint32_t right = 2*index + 2;
        uint32_t highest = index;

        //交换index顺序 
        if (left < pq->size && is_higher(&pq->heap[left], &pq->heap[highest])) {
            highest = left;
        }
        if (right < pq->size && is_higher(&pq->heap[right], &pq->heap[highest])) {
            highest = right;
        }

        //上面这个if满足一个就会触发这个if
        if (highest != index) {
            heap_swap(&pq->heap[index], &pq->heap[highest]);
            index = highest;
        }
        else {
            break;
        }
    }
}



//这样不就只能创建一个static 静态变量吗？
PrioQueue_t* pq_create(void)
{
    static struct PrioQueue_t pq;

    pq.size = 0;
    pq.seq_counter = 0;
    pq.mutuex = xSemaphoreCreateMutex();
    pq.sem = xSemaphoreCreateCounting(PQ_MAX_CAPACITY, 0);

    if (pq.mutuex == NULL || pq.sem == NULL) {
        return NULL;
    }
    return &pq;
}


BaseType_t pq_push(PrioQueue_t* pq,const struct event_t* event,TickType_t timeout)
{
    if (pq == NULL || event == NULL) {
        return pdFAIL;
    }
    if (xSemaphoreTake(pq->mutuex,timeout) != pdPASS) {
        return pdFAIL;
    }

    if (pq->size >= PQ_MAX_CAPACITY) {
        xSemaphoreGive(pq->mutuex);
        return errQUEUE_FULL;
    }

    pq->heap[pq->size] = *event;
    pq->heap[pq->size].seq = pq->seq_counter++;

    heap_bubble_up(pq, pq->size);
    pq->size++;

    xSemaphoreGive(pq->mutuex);
    xSemaphoreGive(pq->sem);

    return pdPASS;
}

BaseType_t pq_pushfromIsr(PrioQueue_t *pq, const struct event_t *event, BaseType_t *pxHigherprioTaskWoken)
{
    if (pq == NULL || event == NULL) {
        return pdFAIL;
    }

    UBaseType_t uxSavedInterruptStautus = taskENTER_CRITICAL_FROM_ISR();
    
    if (pq->size >= PQ_MAX_CAPACITY) {
        taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStautus);
        return errQUEUE_FULL;
    }

    pq->heap[pq->size] = *event;
    pq->heap[pq->size].seq = pq->seq_counter++;
    heap_bubble_up(pq, pq->size);
    pq->size++;

    taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStautus);
    xSemaphoreGiveFromISR(pq->sem, pxHigherprioTaskWoken);
    return pdPASS;
}

BaseType_t pq_pop(PrioQueue_t *pq, struct event_t *out_event, TickType_t timeout)
{
    if (pq == NULL || out_event == NULL) {
        return pdFAIL;
    }              

    if (xSemaphoreTake(pq->sem, timeout) != pdPASS) {
        return pdFAIL;
    }

    xSemaphoreTake(pq->mutuex, portMAX_DELAY);

    if (pq->size == 0) {
        xSemaphoreGive(pq->mutuex);
        return pdFAIL;
    }

    *out_event = pq->heap[0];
    pq->size--;
    if (pq->size > 0) {
        pq->heap[0] = pq->heap[pq->size];
        heap_bubble_down(pq, 0);
    }
    xSemaphoreGive(pq->mutuex);
    return pdPASS;
}



