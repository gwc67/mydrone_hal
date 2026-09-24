#ifndef __UART_TRUE_H
#define __UART_TRUE_H


#include "main.h"
#include "ring_buffer.h"
#include "uart_base.h"

//方便初始化使用的而定义的临时结构体
struct uart_cfg_t
{
    uint32_t tx_len32;  
    uint32_t rx_len32;  
    uint32_t rx_ring_len32;  
    uint32_t tx_ring_len32;  
    uint8_t *tx_data;                          
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
    volatile bool is_busy_b;
    uint8_t *tx_data;                           //hal_transmit_it 使用的指针
    uint8_t *rx_data;
    uint32_t tx_len32;
    uint32_t rx_len32;
    void* user_data;                            //user_data通过回调进一步给出,user_data即 匿名协议的base指针  //对于无协议串口只需要添加一个NULL即可
    uart_callback_t callback;
    uint32_t old_pos32;                        //为dma循环模式下设计的变量
};

int uart_it_init(struct uart_device_t* me,const struct uart_cfg_t* cfg, const char *name);
int uart_dma_init(struct uart_device_t* me,const struct uart_cfg_t* cfg, const char *name);

#endif