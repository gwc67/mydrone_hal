// #include "ring_uart.h"
// #include <stdbool.h>
// #include <stdint.h>



// struct ring_uart_t{
//     struct ring_buf_base_t base;
//     struct ring_buf ring_buf_st;
// };


// #define UT2_RX_RING_BUF 512
// #define UT2_TX_RING_BUF 512

// #define UT3_RX_RING_BUF 512
// #define UT3_TX_RING_BUF 512



// static struct ring_uart_t s_ut2_rx_st;
// static struct ring_uart_t s_ut2_tx_st;
// static uint8_t s_ut2_rx_buf_puc[UT2_RX_RING_BUF];
// static uint8_t s_ut2_tx_buf_puc[UT2_TX_RING_BUF];
// struct ring_buf_base_t* g_ut2_rx_ring_pst;
// struct ring_buf_base_t* g_ut2_tx_ring_pst;


// static struct ring_uart_t s_ut3_rx_st;
// static struct ring_uart_t s_ut3_tx_st;
// static uint8_t s_ut3_rx_buf_puc[UT3_RX_RING_BUF];
// static uint8_t s_ut3_tx_buf_puc[UT3_TX_RING_BUF];

// struct ring_buf_base_t* g_ut3_rx_ring_pst;
// struct ring_buf_base_t* g_ut3_tx_ring_pst;


// static void my_ring_buf_init(struct ring_uart_t* me, uint8_t* ring_buf_puc,uint32_t size_ul,const char* name)
// {
//     me->base.name = name;
//     ring_buf_init(&me->ring_buf_st,size_ul,ring_buf_puc);
// }


// int my_ring_buf_board_init(void)
// {
//     my_ring_buf_init(&s_ut2_rx_st,s_ut2_rx_buf_puc,UT2_RX_RING_BUF, "ut2_rx");
//     g_ut2_rx_ring_pst = &s_ut2_rx_st.base;

//     my_ring_buf_init(&s_ut2_tx_st,s_ut2_tx_buf_puc,UT2_TX_RING_BUF, "ut2_tx");
//     g_ut2_tx_ring_pst = &s_ut2_tx_st.base;

//     my_ring_buf_init(&s_ut3_tx_st,s_ut3_tx_buf_puc,UT3_TX_RING_BUF, "ut3_tx");
//     g_ut3_tx_ring_pst = &s_ut3_tx_st.base;

//     my_ring_buf_init(&s_ut3_rx_st,s_ut3_rx_buf_puc,UT3_RX_RING_BUF, "ut3_rx");
//     g_ut3_rx_ring_pst = &s_ut3_rx_st.base;

//     return 0;
// }

// SYS_INIT(my_ring_buf_board_init, APPLICATION, 10);


// uint32_t my_ring_buf_get(struct ring_buf_base_t* base,uint8_t* data_puc,uint32_t size_ul)
// {
//     struct ring_uart_t* me =  CONTAINER_OF(base, struct my_ring_t , base);
//     return ring_buf_get(&me->ring_buf_st,data_puc, size_ul);

// }

// uint32_t my_ring_buf_put(struct ring_buf_base_t* base,uint8_t* data_puc,uint32_t size_ul)
// {
//     struct ring_uart_t* me = CONTAINER_OF(base, struct my_ring_t, base);
//     return ring_buf_put(&me->ring_buf_st, data_puc, size_ul);
// }

// bool my_ring_buf_is_empty(struct ring_buf_base_t* base)
// {
//     struct ring_uart_t* me = CONTAINER_OF(base, struct my_ring_t, base);
//     return  ring_buf_is_empty(&me->ring_buf_st);
// }
