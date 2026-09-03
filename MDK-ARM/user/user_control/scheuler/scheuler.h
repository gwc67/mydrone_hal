#ifndef __SCHEULER_H
#define __SCHEULER_H
#include "stdint.h"
#include  "main.h"
enum uart_event_type_e
{
    UART_EVENT_TX_REQ  = 0,
    UART_EVENT_TX_BUSY,
    UART_EVENT_TX_DONE,
    UART_EVENT_RX_DATA,
};

struct uart_event_t {
    enum uart_event_type_e type_e;
    uint8_t* data_puc;
    size_t size;          
};

#endif