#ifndef __RING_Q_H
#define __RING_Q_H

#include "ring_buffer.h"
#include "mesc.h"
struct sys_ringq {
	/** @cond INTERNAL_HIDDEN */
	struct ring_buf rb;
	ring_buf_idx_t item_size;
	/** @endcond */
};

#define SYS_RINGQ_INIT(buf, item_sz, item_capacity)			\
{									\
	.rb = RING_BUF_INIT((buf), (item_sz) * (item_capacity)),	\
	.item_size = (item_sz),						\
}

static inline void sys_ringq_init(struct sys_ringq* ringq,uint8_t *data,size_t data_size,
                size_t item_size)
{
    assert_param(data != NULL);
    assert_param(item_size > 0);


    data_size = (data_size / item_size) * item_size;
    assert_param(data_size > 0);

    ring_buf_init(&ringq->rb, data_size, data);
    ringq->item_size = item_size;
    
}



static inline void sys_ringq_reset(struct sys_ringq *ringq)
{
	ring_buf_reset(&ringq->rb);
}

static inline bool sys_ringq_empty(const struct sys_ringq *ringq)
{
	return ring_buf_is_empty(&ringq->rb);
}

static inline bool sys_ringq_full(const struct sys_ringq *ringq)
{
	return ring_buf_space_get(&ringq->rb) == 0;
}


static inline size_t sys_ringq_space(const struct sys_ringq *ringq)
{
	return ring_buf_space_get(&ringq->rb) / ringq->item_size;
}

static inline size_t sys_ringq_capacity(const struct sys_ringq *ringq)
{
	return ring_buf_capacity_get(&ringq->rb) / ringq->item_size;
}

static inline size_t sys_ringq_size(const struct sys_ringq *ringq)
{
	return ring_buf_size_get(&ringq->rb) / ringq->item_size;
}

static inline int sys_ringq_put(struct sys_ringq *ringq, const void *element)
{
	return ring_buf_put(&ringq->rb, (const uint8_t *)element, ringq->item_size) == 0 ?
		-ENOSPC : 0;
}

static inline int sys_ringq_get(struct sys_ringq *ringq, void *element)
{
	return ring_buf_get(&ringq->rb, (uint8_t *)element, ringq->item_size) == 0 ? -ENODATA : 0;
}

static inline int sys_ringq_peek(struct sys_ringq *ringq, void *data)
{
	return ring_buf_peek(&ringq->rb, (uint8_t *)data, ringq->item_size) == 0 ? -ENODATA : 0;
}

#endif