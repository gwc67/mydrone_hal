#ifndef __UART_TRUE_H
#define __UART_TRUE_H


#include "main.h"
#include "ring_buffer.h"
#include "uart_base.h"

//中断调用一个根据函数反推base句柄

struct uart_cfg_t
{
    uint32_t tx_len32;  
    uint32_t rx_len32;  
    uint32_t rx_ring_len32;  
    uint32_t tx_ring_len32;  
    uint8_t *tx_data;                           //hal_transmit_it 使用的指针
    uint8_t *rx_data;
    uint8_t* rx_ring_data;
    uint8_t* tx_ring_data;
    UART_HandleTypeDef* uart_handle;
};

struct uart_device_t
{
    struct uart_base_t base;
    UART_HandleTypeDef *uart_handle;
    struct ring_buf tx_ring;
    struct ring_buf rx_ring;
    uint8_t *tx_data;                           //hal_transmit_it 使用的指针
    uint8_t *rx_data;
    uint32_t tx_len32;
    uint32_t rx_len32;
};

int uart_it_init(struct uart_device_t* me,const struct uart_cfg_t* cfg, const char *name);

#endif