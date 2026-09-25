#include "double_tree.h"
#include "event.h"
#include "driver_registry.h"
struct PrioQueue_t {
    struct event_t heap[PQ_MAX_CAPACITY];
    uint32_t size;
    uint32_t seq_counter;           //全局递增序列号
    SemaphoreHandle_t mutex;   
    SemaphoreHandle_t sem;          //计数信号量，用于阻塞等待
};

static struct PrioQueue_t s_pq;

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
void pq_init(void)
{
    s_pq.size = 0;
    s_pq.seq_counter = 0;
    s_pq.mutex = xSemaphoreCreateMutex();
    s_pq.sem = xSemaphoreCreateCounting(PQ_MAX_CAPACITY, 0);

    if (s_pq.mutex == NULL || s_pq.sem == NULL) {
        return;
    }
}

DRIVER_INIT_1(pq_init);

BaseType_t pq_push_simple(enum event_id_e id,
  enum event_prio_e prio,TickType_t timeout)
{

    if (xSemaphoreTake(s_pq.mutex,timeout) != pdPASS) {
        return pdFAIL;
    }

    if (s_pq.size >= PQ_MAX_CAPACITY) {
        xSemaphoreGive(s_pq.mutex);
        return errQUEUE_FULL;
    }

    struct event_t event = {.id = id,.prio = prio};

    s_pq.heap[s_pq.size] = event;
    s_pq.heap[s_pq.size].seq = s_pq.seq_counter++;

    heap_bubble_up(&s_pq, s_pq.size);
    s_pq.size++;

    xSemaphoreGive(s_pq.mutex);
    xSemaphoreGive(s_pq.sem);

    return pdPASS;
}

BaseType_t pq_push(const struct event_t* event,TickType_t timeout)
{
     if (xSemaphoreTake(s_pq.mutex,timeout) != pdPASS) {
        return pdFAIL;
    }

    if (s_pq.size >= PQ_MAX_CAPACITY) {
        xSemaphoreGive(s_pq.mutex);
        return errQUEUE_FULL;
    }

    s_pq.heap[s_pq.size] = *event;
    s_pq.heap[s_pq.size].seq = s_pq.seq_counter++;

    heap_bubble_up(&s_pq, s_pq.size);
    s_pq.size++;

    xSemaphoreGive(s_pq.mutex);
    xSemaphoreGive(s_pq.sem);

    return pdPASS;
}

BaseType_t pq_pushfromIsr(const struct event_t *event, BaseType_t *pxHigherprioTaskWoken)
{
    UBaseType_t uxSavedInterruptStautus = taskENTER_CRITICAL_FROM_ISR();

    if (s_pq.size >= PQ_MAX_CAPACITY) {
        taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStautus);
        return errQUEUE_FULL;
    }

    s_pq.heap[s_pq.size] = *event;
    s_pq.heap[s_pq.size].seq = s_pq.seq_counter++;
    heap_bubble_up(&s_pq, s_pq.size);
    s_pq.size++;

    taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStautus);
    xSemaphoreGiveFromISR(s_pq.sem, pxHigherprioTaskWoken);
    return pdPASS;
}

BaseType_t pq_pop(struct event_t *out_event, TickType_t timeout)
{
    if (out_event == NULL) {
        return pdFAIL;
    }

    if (xSemaphoreTake(s_pq.sem, timeout) != pdPASS) {
        return pdFAIL;
    }

    xSemaphoreTake(s_pq.mutex, portMAX_DELAY);

    if (s_pq.size == 0) {
        xSemaphoreGive(s_pq.mutex);
        return pdFAIL;
    }

    *out_event = s_pq.heap[0];
    s_pq.size--;
    if (s_pq.size > 0) {
        s_pq.heap[0] = s_pq.heap[s_pq.size];
        heap_bubble_down(&s_pq, 0);
    }
    xSemaphoreGive(s_pq.mutex);
    return pdPASS;
}




