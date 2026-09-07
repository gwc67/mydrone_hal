#ifndef  __ANO_TRUE_H
#define  __ANO_TRUE_H

#include <stdint.h>
#include "ano_base.h"
#define FRAME_NUM_LEN 256       /* 0~0xff 共256个帧ID */
#define FRAME_MAX_LENGTH 64

// struct frame_t
// {
//     uint8_t address;
//     uint8_t wts_uc;
//     uint16_t fre_ms_us;
//     uint16_t time_cnt_ms;
// };


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

// //私有的函数指针
// typedef struct
// {
//     void (*ano_receive_anl)(uint8_t* data_puc,uint8_t len_uc);
//     void (*ano_add_send_data)(uint8_t frame_num_uc,uint8_t *cnt_puc,uint8_t* data_puc);
//     void (*ano_send_buffer)(uint8_t *data_puc,uint8_t len_uc);
// }private_t;


struct ano_cfg_t {
    uint8_t* rx_buffer_puc;
    void (*ano_receive_anl)(uint8_t* data,uint8_t len8);
    void (*ano_add_send_data)(uint8_t frame,uint8_t *cnt_ptr,uint8_t* data);
    void (*ano_send_buffer)(uint8_t *data,uint8_t len8);    //不同设备的指针对应的特殊指针
};


struct ano_device_t
{
    struct ano_base_t base;
    struct ano_frame_t* ano_frame_pst;
    const struct ano_cfg_t* ano_cfg_pst;
    
    uint8_t rx_state_uc;
    uint8_t data_len_uc;
    uint8_t data_cnt_uc;
};

// int ano_device_init_noraml(struct ano_device_t* me,struct ano_frame_t* ano_frame_pst,const struct ano_cfg_t* ano_cfg_pst);


#endif
