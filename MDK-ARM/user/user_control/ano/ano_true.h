#ifndef  __ANO_TRUE_H
#define  __ANO_TRUE_H

#include <stdint.h>
#include "ano_base.h"
#include "uart_base.h"
#define FRAME_NUM_LEN 256       /* 0~0xff 共256个帧ID */
#define FRAME_MAX_LENGTH 64
struct check_repeat_t
{
    uint8_t wait_ck;
    uint8_t repeat_cnt_uc;
    uint8_t time_dly_uc;
};

struct ano_frame_t
{
    struct ano_event_t ano_event_pst[FRAME_NUM_LEN];
    struct check_repeat_t check_repeat_st;      
    struct cmd_t send_cmd_st;
    struct ck_t send2check_st;
    struct ck_t back2check_st;
    struct par_t par_data_st;
};
// struct ano_cfg_t {
//     // struct uart_base_t* uart_base;
//     uint8_t* rx_buffer;
// };


struct ano_device_t
{
    struct ano_base_t base;
    struct ano_frame_t* frame_pst;
    uint8_t*rx_buffer;
    // const struct ano_cfg_t* cfg_pst;
    uint8_t rx_state;
    uint8_t data_len8;
    uint8_t data_cnt8;
    
    ano_add_send_data_t add_send_data;
    ano_send_buffer_t  send_buffer;
    ano_receive_anl_t  receive_anl;

    
    
};
int ano_device_init(struct ano_device_t* me,struct ano_frame_t* frame_pst,uint8_t* rx_buffer,uart_base_t* uart_base, const char* name);


#endif
