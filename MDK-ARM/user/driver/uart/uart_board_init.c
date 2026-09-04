#include "uart_true.h"
#include "usart.h"
#include "driver_registry.h"
#include "mesc.h"
#define RING_TX_LEN 512
#define RING_RX_LEN 512
#define TX_LEN 100
#define RX_LEN 100

static struct uart_device_t s_uart_computer;
static uint8_t s_com_ring_rx[RING_RX_LEN];
static uint8_t s_com_ring_tx[RING_TX_LEN];
static uint8_t s_com_tx[TX_LEN];
static uint8_t s_com_Rx[RX_LEN];


struct uart_base_t* g_uart_computer;



struct uart_base_t* s_uart_get_base(UART_HandleTypeDef** uart_handle_pp)
{
    struct uart_device_t* me = CONTAINER_OF(uart_handle_pp, struct uart_device_t, uart_handle_pp);
    return &me->base;
}


void uart_board_init(void)
{
    
    static UART_HandleTypeDef *huart_com = &huart1;
    uart_it_init(&s_uart_computer, s_com_Rx, s_com_tx, &(huart_com), "uart_computer", s_com_ring_rx, s_com_ring_tx);
    g_uart_computer = &s_uart_computer.base;
    uart_receive_enable(g_uart_computer);

}
DRIVER_INIT(uart_board_init);
