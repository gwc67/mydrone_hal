#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "main.h"
#include "Ano_Math.h"


typedef uint16_t ring_buf_idx_t;            //ring_buf_idx 本质是 uint16_t

#define RING_BUFFER_MAX_SIZE (UINT16_MAX / 2)
#define RING_BUFFER_SIZE_ASSERT_MSG "Size too big, please enable CONFIG_RING_BUFFER_LARGE"

struct ring_buf_index {ring_buf_idx_t head,tail,base;};

//解决原子性问题，可以描述 正在写入数据，但对消费者而言无法进行读取的状态，保证消费者每次读到的都是完整的数据，而不是来一个读一个的非原子性设计
//get.head 永远在“追赶” put.tail（已提交的数据）。
// put.head 永远在“追赶” get.tail（已读取释放的空间）。
struct ring_buf {
	/** @cond INTERNAL_HIDDEN */
	uint8_t *buffer;
	struct ring_buf_index put;
	struct ring_buf_index get;
	uint32_t size;
	/** @endcond */
};

uint32_t ring_buf_area_claim(struct ring_buf *buf, struct ring_buf_index *ring,
			     uint8_t **data, uint32_t size);
int ring_buf_area_finish(struct ring_buf *buf, struct ring_buf_index *ring,
			 uint32_t size);
//static 是用于防止函数在.h定义出现的链接问题 
//inline 可以优化函数性能，直接进行展开
//uint16_t的值 value 进行操作
static inline void ring_buf_internal_reset(struct ring_buf* buf,ring_buf_idx_t value)
{
    buf->put.head = buf->put.tail = buf->put.base = value;
    buf->get.head = buf->get.tail = buf->get.base = value;
}

#define RING_BUF_INIT(buf, size8)	\
{					\
	.buffer = (buf),		\
	.size = (size8),		\
}


static inline void ring_buf_init(struct ring_buf *buf,
				 uint32_t size,
				 uint8_t *data)
{

    
    // 不懂设计哲学
	assert_param(size <= RING_BUFFER_MAX_SIZE);  //size > ring_buffer_max_size时值等于0 ，进入断言
	buf->size = size;
	buf->buffer = data;
	ring_buf_internal_reset(buf, 0);
}

//只要仿照这个item的思路就可以写出不错的东西
static inline void ring_buf_item_init(struct ring_buf *buf,
				      uint32_t size,
				      uint32_t *data)
{
	assert_param(size <= RING_BUFFER_MAX_SIZE / 4);
	ring_buf_init(buf, 4 * size, (uint8_t *)data);
}

static inline bool ring_buf_is_empty(const struct ring_buf *buf)
{
	return buf->get.head == buf->put.tail;
}


static inline void ring_buf_reset(struct ring_buf *buf)
{
	ring_buf_internal_reset(buf, 0);
}

//return available_space get-tail在追赶 put-tail
//可读的字节数量
static inline uint32_t ring_buf_size_get(const struct ring_buf *buf)
{
	ring_buf_idx_t available = buf->put.tail - buf->get.head;

	return available;
}
/**
 * @brief Determine free space in a ring buffer.
 *
 * @param buf Address of ring buffer.
 *
 * @return Ring buffer free space (in bytes).
 */
static inline uint32_t ring_buf_space_get(const struct ring_buf *buf)
{
	ring_buf_idx_t allocated = buf->put.head - buf->get.tail;

	return buf->size - allocated;
}

//获得item的大小
static inline uint32_t ring_buf_item_space_get(const struct ring_buf *buf)
{
	return ring_buf_space_get(buf) / 4;
}

//返回ring_buf绑定的实际数组的大小
static inline uint32_t ring_buf_capacity_get(const struct ring_buf *buf)
{
	return buf->size;
}

static inline int ring_buf_put_finish(struct ring_buf *buf, uint32_t size)
{
	return ring_buf_area_finish(buf, &buf->put, size);
}


//&buf->put space = freespace
static inline uint32_t ring_buf_put_claim(struct ring_buf *buf,
					  uint8_t **data,
					  uint32_t size)
{
	uint32_t space = ring_buf_space_get(buf);
	return ring_buf_area_claim(buf, &buf->put, data,
				   _MIN(size, space));
}

//&buf->get 
static inline uint32_t ring_buf_get_claim(struct ring_buf *buf,
					  uint8_t **data,
					  uint32_t size)
{
	uint32_t buf_size = ring_buf_size_get(buf);
	return ring_buf_area_claim(buf, &buf->get, data,
				   _MIN(size, buf_size));
}

//&buf->get 
static inline int ring_buf_get_finish(struct ring_buf *buf, uint32_t size)
{
	return ring_buf_area_finish(buf, &buf->get, size);
}


uint32_t ring_buf_put(struct ring_buf *buf, const uint8_t *data, uint32_t size);
uint32_t ring_buf_get(struct ring_buf *buf, uint8_t *data, uint32_t size);
uint32_t ring_buf_peek(struct ring_buf *buf, uint8_t *data, uint32_t size);

#endif