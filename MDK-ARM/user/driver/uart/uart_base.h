
#ifndef     __UART_BASE_H
#define     __UART_BASE_H

#include <stdint.h>
#include "main.h"
#include "usart.h"
typedef struct uart_base_t uart_base_t ;

typedef  int (*uart_callback_t)(uint8_t* data,uint32_t len32,void* user_data);

typedef struct {
   int (*uart_transmit)(uart_base_t* me,uint8_t* data ,uint32_t len32);
   int (*uart_rx_isr)(uart_base_t* me,uint32_t len32);
   int (*uart_rx_enable)(uart_base_t* me);                  //由于是异步的，故这个是设置多久没数据判断为空闲状态     
   UART_HandleTypeDef* (*uart_get_handle)(uart_base_t* me);
   int (*uart_rx_analyze)(uart_base_t*me);
   int (*uart_register_callback)(uart_base_t* me,uart_callback_t callback,void* user_data);
}uart_ops_t;

struct uart_base_t {
   const char* name;
   const uart_ops_t* ops;
};

enum uart_event_type_e
{
    UART_EVENT_TX_REQ  = 0,
    UART_EVENT_TX_DONE,
    UART_EVENT_RX_DATA,
};

//使用uart_event_t 创建 TX，RX两个消息队列
struct uart_event_t {
    enum uart_event_type_e type_e;
    struct uart_base_t* base;
    uint8_t* data_ptr;
    size_t size;          
};


int uart_transmit(uart_base_t* me,uint8_t* data_puc ,uint32_t len32);
int uart_receive_enable(uart_base_t* me);
int uart_rx_isr(uart_base_t* me,uint32_t len32);
UART_HandleTypeDef* uart_get_handle(uart_base_t* me);
int uart_register_callback(uart_base_t* me,uart_callback_t callback,void* user_data);
int uart_rx_analyze(uart_base_t*me);
#endif
