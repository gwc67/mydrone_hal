#include "ring_buffer.h"
#include "mesc.h"
#include "Ano_Math.h"


//return index的head的位置 输出项data 即指向ring_buf head的位置
uint32_t ring_buf_area_claim(struct ring_buf* buf,struct ring_buf_index* ring,uint8_t **data,uint32_t size)
{
    ring_buf_idx_t head_offset, wrap_size;

    // 计算head相对于base的偏移量
    head_offset = ring->head - ring->base;
    // 如果head回绕到了上一轮,size范围内, 则减去size得到实际偏移
    if (unlikely(head_offset >= buf->size)) {
        head_offset -= buf->size;
    }

    // 计算当前head到缓冲区末尾的剩余连续空间
    wrap_size = buf->size - head_offset;
    // 请求大小不能超过连续空间, 否则截断(避免数据跨区绕回)
    size = _MIN(size,wrap_size);
    // 返回当前head位置的指针供写入
    *data = &buf->buffer[head_offset];
    // 推进head, 预留空间(但tail未变, 数据尚不可见)
    ring->head += size;

    return size;
}

/**
 *
 * @param buf  环形缓冲区指针
 * @param ring 操作索引指针(put或get), 用于追踪head/tail/base
 * @param size 要提交的数据字节数(不能超过claim时分配的大小)
 *
 * @return 0 成功; -EINVAL size超过已claim的大小 claim完 需要进行 finish
 */
 //
int ring_buf_area_finish(struct ring_buf *buf, struct ring_buf_index *ring,
			 uint32_t size)
{
	ring_buf_idx_t claimed_size, tail_offset;

	// 计算当前已claim但未提交的字节数
	claimed_size = ring->head - ring->tail;
	// 提交大小不能超过已claim大小, 否则返回错误
	if (unlikely(size > claimed_size)) {
		return -EINVAL;
	}

	// 推进tail, 使size字节数据对读取侧可见
	ring->tail += size;
	// head回退到tail, 清空未提交区域
	ring->head = ring->tail;

	// 计算tail相对于base的偏移
	tail_offset = ring->tail - ring->base;
	// 如果tail越过缓冲区边界, 调整base实现回绕
	if (unlikely(tail_offset >= buf->size)) {
		/* we wrapped: adjust ring->base */
		ring->base += buf->size;
	}

	return 0;
}


//优雅处理环形环绕（wrap）问题，分两次写入
// return 实际写入的数据量
uint32_t ring_buf_put(struct ring_buf *buf, const uint8_t *data, uint32_t size)
{
    uint8_t *dst;
    uint32_t partial_size;          //分两次写入的时候，各次的长度
    uint32_t total_size = 0U;
    int err;


    do {
        partial_size = ring_buf_put_claim(buf,&dst,size);
        if (partial_size == 0) {
            break;
        }
        memcpy(dst,data,partial_size);
        total_size += partial_size;
        size -= partial_size;      //剩下size-partial_size 没有写入
        data += partial_size;      //data往前进步 partial_size
    } while (size != 0);    

    err = ring_buf_put_finish(buf, total_size);         //0 success 

    assert_param(err == 0);
    ARG_UNUSED(err);

    return total_size;

}

uint32_t ring_buf_get(struct ring_buf *buf, uint8_t *data, uint32_t size)
{
    uint8_t *src;
    uint32_t partial_size;
    uint32_t total_size = 0U;
    int err;

    do {
        partial_size = ring_buf_get_claim(buf, &src, size);  //返回的是get.head 的指针 get.head 需要追赶 put.tail
        if (partial_size == 0) {
            break;
        }
        if (data) {
            memcpy(data, src, partial_size);
            data += partial_size;
        }
        total_size += partial_size;
        size -= partial_size;
    }while(size != 0);

    err = ring_buf_get_finish(buf, total_size);         //0 success
    assert_param(err == 0);
    ARG_UNUSED(err);
    return total_size;
}


uint32_t ring_buf_peek(struct ring_buf *buf, uint8_t *data, uint32_t size)
{
	uint8_t *src;
	uint32_t partial_size;
	uint32_t total_size = 0U;
	int err;

	do {
		partial_size = ring_buf_get_claim(buf, &src, size);
		if (partial_size == 0) {
			break;
		}
		assert_param(data != NULL);
		memcpy(data, src, partial_size);
		data += partial_size;
		total_size += partial_size;
		size -= partial_size;
	} while (size != 0);

	/* effectively unclaim total_size bytes */
	err = ring_buf_get_finish(buf, 0);              //这个0 和 ring_buf_get的本质区别
	assert_param(err == 0);
	ARG_UNUSED(err);

	return total_size;
}

//Bit Field
struct ring_element {
	uint32_t  type   :16; /**< Application-specific */
	uint32_t  length :8;  /**< length in 32-bit chunks */
	uint32_t  value  :8;  /**< Room for small integral values */
};


//这里先放入了一个ring_element 之后再把具体的data32 放进去
int ring_buf_item_put(struct ring_buf* buf,uint16_t type,uint8_t value,uint32_t *data32,uint8_t size32)
{
    uint8_t *dst, *data = (uint8_t *)data32;  // *data 是指向data32 的指针
    struct ring_element* header;
    uint32_t space,size,partial_size,total_size;
    int err;

    space = ring_buf_space_get(buf);   //获取剩余空间
    size = size32 * 4;
    if (size + sizeof(struct ring_element ) > space) {
        return -EMSGSIZE;
    }

    //先申请ring_buf一块dst所指向的区域，大小是ring_element(4 byte) 存放ring_element的头
    err = ring_buf_put_claim(buf,&dst,sizeof(struct ring_element));
    assert_param(err == sizeof(struct ring_element));

    //通过header 对dst所指向的指针进行赋值
    header = (struct ring_element*)dst;
    header->type = type;
    header->length = size32;
    header->value = value;
    total_size = sizeof(struct ring_element);

    do {
        //size = size32 * 4
        partial_size = ring_buf_put_claim(buf, &dst, size);
        if (partial_size == 0) {
            break;
        }
        memcpy(dst, data, partial_size);
        size -= partial_size;
        total_size += partial_size;     
        data += partial_size;           //数组指针也向前移动partial_size
    }while (size != 0);
    assert_param(size == 0);

    err = ring_buf_put_finish(buf, total_size);

    assert_param(err);
    ARG_UNUSED(err);

    return 0;


}

int ring_buf_item_get(struct ring_buf* buf,uint16_t* type,uint8_t *value,uint32_t* data32,uint8_t *size32)
{
    uint8_t *src, *data = (uint8_t *)data32;
    struct ring_element *header;
    uint32_t size,partial_size,total_size;
    int err;

    if (ring_buf_is_empty(buf)) {
        return -EAGAIN;
    }

    err = ring_buf_get_claim(buf, &src, sizeof(struct ring_element));
    assert_param(err == sizeof(struct ring_element)); 
    
    //先取一个头出来
    header = (struct ring_element* )src;
    if (data && (header->length > *size32)) {
        *size32 = header->length;
        ring_buf_get_finish(buf, 0);
        return -EMSGSIZE;
    }

    *size32 = header->length;
    *type = header->type;
    *value = header->value;

    total_size = sizeof(struct ring_element);

    size = *size32 * 4;

    do {
        partial_size = ring_buf_get_claim(buf, &src, size);
        if (partial_size == 0) {
            break;
        }
        if (data) {
            memcpy(data, src, partial_size);
            data += partial_size;
        }
        total_size += partial_size;
        size -= partial_size;
        
    }while (size != 0);

    err = ring_buf_get_finish(buf, total_size);
    assert_param(err == 0);
    ARG_UNUSED(err);

    return 0;
}