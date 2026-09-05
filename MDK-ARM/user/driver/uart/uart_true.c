#include "uart_true.h"
#include "mesc.h"
#include "FreeRTOS.h"
#include "semphr.h"

extern QueueHandle_t uart_tx_queue;
extern QueueHandle_t uart_rx_queue;

static int s_uart_tx_it(struct uart_base_t* base,uint8_t *data,uint32_t len32)
{
    struct uart_device_t *me = CONTAINER_OF(base, struct uart_device_t, base);
    
    ring_buf_put(&me->tx_ring, data, len32);
    
    struct uart_event_t event = {
        .base = base,
        .type_e = UART_EVENT_TX_REQ,
    };
    return xQueueSend(uart_tx_queue,&event,0);
}


//这个应该是一个释放消息的，并没有改变什么东西，需要改变串口是否忙的状态，那不是要又要创建一个接口吗？我认为多余了
//可以学习，在写一个uart_tx_event的自带封装即可
static int s_uart_tx_isr_it(struct uart_base_t *base)
{
    BaseType_t xtaskwoken = pdFALSE;
    struct uart_event_t event = {
        .base = base,
        .type_e = UART_EVENT_TX_DONE,
    };
    xQueueSendFromISR(uart_tx_queue, &event, &xtaskwoken);
    portYIELD_FROM_ISR(xtaskwoken);
    return 0;
}

//为了解决共享线程中，无法进行tx_busy标志位的判断
//tx_busy 主要解决 刚调用HAL_Transmit_it传输数据，数据还没发送完，系统再次调用 HAL_Transmit_it产生的覆盖问题
//tx_busy 主要约束 EVENT_REQ事件
static int s_uart_tx_callback(struct uart_base_t* base,enum uart_event_type_e event)
{
    struct uart_device_t* me = CONTAINER_OF(base, struct uart_device_t, base);
    if (event == UART_EVENT_TX_REQ) {
        if (!me->is_busy_b) {
            uint32_t len = ring_buf_get(&me->tx_ring, me->tx_data, me->tx_len32);
            HAL_UART_Transmit_IT(me->uart_handle, me->tx_data, len);
            me->is_busy_b = true;
        }
    }
    else if (event == UART_EVENT_TX_DONE) {
        if (ring_buf_is_empty(&me->tx_ring)) {
            me->is_busy_b = false;
        }
        else {
            uint32_t len = ring_buf_get(&me->tx_ring, me->tx_data, me->tx_len32);
            HAL_UART_Transmit_IT(me->uart_handle, me->tx_data, len);
            me->is_busy_b = true;
        }
    }
    return 0;
}


//这是给解析线程使用的
static int s_uart_rx_isr_it(struct uart_base_t* base,uint32_t len32)
{
    struct uart_device_t* me = CONTAINER_OF(base, struct uart_device_t, base);

    BaseType_t xtaskwoken = pdFALSE;
    ring_buf_put(&me->rx_ring, me->rx_data, len32);

    struct uart_event_t event = {
        .base = base,
        .type_e = UART_EVENT_RX_DATA,
    };
    xQueueSendFromISR(uart_rx_queue, &event, &xtaskwoken);
    portYIELD_FROM_ISR(xtaskwoken);
    return HAL_UARTEx_ReceiveToIdle_IT(me->uart_handle, me->rx_data, me->rx_len32);   
}



//通过引入不同的回调，可以对it，和dma产生只需要一次启动即可，就像zephyr一样
static int s_uart_rx_enalbe_it(struct uart_base_t* base)
{
    struct uart_device_t* me = CONTAINER_OF(base, struct uart_device_t, base);
    return HAL_UARTEx_ReceiveToIdle_IT(me->uart_handle, me->rx_data, me->rx_len32);   
}

static UART_HandleTypeDef* s_uart_get_handle(struct uart_base_t* base)
{
    struct uart_device_t* me = CONTAINER_OF(base, struct uart_device_t, base);
    return me->uart_handle;
}

static int s_uart_rx_analyze(uart_base_t* base)
{
    struct uart_device_t* me = CONTAINER_OF(base, struct uart_device_t, base);  

    if (me->callback == NULL) {
        return -EINVAL;
    }

    uint8_t data[100];
    
    while (!ring_buf_is_empty(&me->rx_ring)) {
        uint32_t len = ring_buf_get(&me->rx_ring,data,sizeof(data));
        me->callback(data,len,me->user_data);
    };
    return 0;
}    
    
static int s_uart_callback_register(uart_base_t* base,uart_callback_t callback,void* user_data)
{
    struct uart_device_t* me = CONTAINER_OF(base, struct uart_device_t, base);  
    me->user_data = user_data;
    me->callback = callback;
    return 0;
}
const uart_ops_t uart_ops_it = {
    .uart_rx_enable = s_uart_rx_enalbe_it,
    .uart_transmit = s_uart_tx_it,
    .uart_rx_isr = s_uart_rx_isr_it,
    .uart_get_handle = s_uart_get_handle,
    .uart_register_callback = s_uart_callback_register,
    .uart_rx_analyze = s_uart_rx_analyze,
    .uart_tx_isr = s_uart_tx_isr_it,
    .uart_tx_callback = s_uart_tx_callback,
};

int uart_it_init(struct uart_device_t* me,const struct uart_cfg_t* cfg, const char *name)
{
    if (!me || !cfg->rx_data || !cfg->tx_data || !cfg->rx_ring_data || !cfg->tx_ring_data) {
        return -EINVAL;
    }
    me->base.name = name;
    me->base.ops = &uart_ops_it;
    me->uart_handle = cfg->uart_handle;
    
    ring_buf_init(&me->rx_ring,cfg->rx_ring_len32,cfg->rx_ring_data);
    ring_buf_init(&me->tx_ring,cfg->tx_ring_len32,cfg->tx_ring_data);

    me->rx_data = cfg->rx_data;
    me->tx_data = cfg->tx_data;

    me->tx_len32 = cfg->tx_len32;
    me->rx_len32 = cfg->rx_len32;


    return  0;
}

