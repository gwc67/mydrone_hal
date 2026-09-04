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
        .me = base,
        .data_ptr = NULL,
        .size = 0,
        .type_e = UART_EVENT_TX_REQ,
    };
    return xQueueSend(uart_tx_queue,&event,0);
}

//这是给解析线程使用的
static int s_uart_rx_isr_it(struct uart_base_t* base,uint32_t len32)
{
    struct uart_device_t* me = CONTAINER_OF(base, struct uart_device_t, base);

    BaseType_t xtaskwoken = pdFALSE;
    ring_buf_put(&me->rx_ring, me->rx_data, len32);

    struct uart_event_t event = {
        .me = base,
        .data_ptr = NULL,
        .size = 0,
        .type_e = UART_EVENT_RX_DATA,
    };
    xQueueSendFromISR(uart_tx_queue, &event, &xtaskwoken);
    portYIELD_FROM_ISR(xtaskwoken);
    return HAL_UARTEx_ReceiveToIdle_IT(*me->uart_handle_pp, me->rx_data, sizeof(me->rx_data));   
}

//通过引入不同的回调，可以对it，和dma产生只需要一次启动即可，就像zephyr一样
static int s_uart_rx_enalbe_it(struct uart_base_t* base)
{
    struct uart_device_t* me = CONTAINER_OF(base, struct uart_device_t, base);
    return HAL_UARTEx_ReceiveToIdle_IT(*me->uart_handle_pp, me->rx_data, sizeof(me->rx_data));   
}


const uart_ops_t uart_ops_it = {
    .uart_rx_enable = s_uart_rx_enalbe_it,
    .uart_transmit = s_uart_tx_it,
    .uart_rx_isr = s_uart_rx_isr_it,
};

int uart_it_init(struct uart_device_t* me,const struct uart_cfg_t* cfg, UART_HandleTypeDef **uart_handle_pp,const char *name)
{
    if (!me || !uart_handle_pp || !rx_ring_data || !tx_ring_data) {
        return -EINVAL;
    }
    me->base.name = name;
    me->base.ops = &uart_ops_it;
    me->cfg = cfg;
    ring_buf_init(&me->tx_ring,cfg->rx_ring_len32,tx_ring_data);
    ring_buf_init(&me->rx_ring,sizeof(rx_ring_data),rx_ring_data);
    me->uart_handle_pp = uart_handle_pp;
    me->rx_data = rx_data;
    me->tx_data = tx_data;

    return  0;
}