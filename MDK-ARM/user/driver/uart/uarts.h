#ifndef __UARTS_H
#define __UARTS_H

#include "uart_base.h"

extern struct uart_base_t* g_uart_computer;
struct uart_base_t* s_uart_get_base(UART_HandleTypeDef** uart_handle_pp);


#endif