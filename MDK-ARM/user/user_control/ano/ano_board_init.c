// #include "ano_base.h"
// #include "ano_true.h"
// #include "zephyr/init.h"
// #include <stdint.h>
// #include <string.h>
// #include "ano_device/ano_device_com.h"

// static struct ano_device_t s_com_st;
// static uint8_t s_com_rx_buffer_pst[FRAME_MAX_LENGTH];
// static struct ano_frame_t s_com_frame_st;

// struct ano_base_t* g_com_ano_pst;

// int ano_board_init(void)
// {
//     int result = 0;
//     result = ano_device_init_noraml(&s_com_st, &s_com_frame_st, &s_com_cfg_st);
//     if (result != 0) {
//         return -1;
//     }
    

//     g_com_ano_pst = &s_com_st.base;

//     return result;

// }