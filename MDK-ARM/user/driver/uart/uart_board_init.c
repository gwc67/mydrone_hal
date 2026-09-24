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
__attribute__((section(".dma_buf"))) static uint8_t s_com_tx[TX_LEN];
__attribute__((section(".dma_buf"))) static uint8_t s_com_rx[RX_LEN];

static struct uart_device_t s_uart_lx;
static uint8_t s_lx_ring_rx[RING_RX_LEN];
static uint8_t s_lx_ring_tx[RING_TX_LEN];
__attribute__((section(".dma_buf"))) static uint8_t s_lx_tx[TX_LEN];
__attribute__((section(".dma_buf"))) static uint8_t s_lx_rx[RX_LEN];


struct uart_base_t* g_uart_com;
struct uart_base_t* g_uart_lx;

// struct uart_base_t* s_uart_get_base(UART_HandleTypeDef** uart_handle_pp)
// {
//     struct uart_device_t* me = CONTAINER_OF(uart_handle_pp, struct uart_device_t, uart_handle_pp);
//     return &me->base;
// }


void uart_board_init(void)
{
    
    struct uart_cfg_t com_cfg = {
        .uart_handle = &huart1,
        .rx_data = s_com_rx,
        .tx_data = s_com_tx,
        .rx_ring_data = s_com_ring_rx,
        .tx_ring_data = s_com_ring_tx,
        .tx_len32 = TX_LEN,
        .tx_ring_len32 = RING_TX_LEN,
        .rx_len32 = RX_LEN,
        .rx_ring_len32 = RING_RX_LEN,
    };
    uart_it_init(&s_uart_computer, &com_cfg, "uart_computer");
    g_uart_com = &s_uart_computer.base;
    struct uart_cfg_t lx_cfg = {
        .uart_handle = &huart1,
        .rx_data = s_lx_rx,
        .tx_data = s_lx_tx,
        .rx_ring_data = s_lx_ring_rx,
        .tx_ring_data = s_lx_ring_tx,
        .tx_len32 = TX_LEN,
        .tx_ring_len32 = RING_TX_LEN,
        .rx_len32 = RX_LEN,
        .rx_ring_len32 = RING_RX_LEN,
    };
    
   
}
DRIVER_INIT_1(uart_board_init);

void uart_receive_all_init(void)
{
    uart_receive_enable(g_uart_com);
}
DRIVER_INIT_4(uart_receive_all_init);
