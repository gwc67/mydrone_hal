
#ifndef     __UART_BASE_H
#define     __UART_BASE_H

#include <stdint.h>
typedef struct uart_base_t uart_base_t ;


typedef struct {
   int (*uart_transmit)(uart_base_t* me,uint8_t* data_puc ,uint32_t len_ul);
   int (*uart_rx_enable)(uart_base_t* me,uint32_t time_out_ul);                  //由于是异步的，故这个是设置多久没数据判断为空闲状态     
}uart_ops_t;

struct uart_base_t {
   const uart_ops_t* ops;
};

int uart_transmit(uart_base_t* me,uint8_t* data_puc ,uint32_t len_ul);
int uart_receive_enable(uart_base_t* me,uint32_t time_out_ul);

#endif
