#include "ano_true.h"
#include "ano_base.h"
#include "my_ring.h"
#include <stdint.h>
#include <sys/_intsup.h>
#include <zephyr/sys/util.h>



///转换成ano协议的数据

static int s_data_to_ano(struct ano_device_t *me,uint8_t byte_uc)
{
    if (me->rx_state_uc == 0 && byte_uc == 0xAA)
    {
        me->rx_state_uc = 1;
        me->data_cnt_uc = 0;
        me->data_len_uc = 0;
        me->ano_cfg_pst->rx_buffer_puc[me->data_cnt_uc++] = byte_uc;
    }
    else if (me->rx_state_uc == 1 && byte_uc == 0xFF)
    {
        me->rx_state_uc = 2;
        me->ano_cfg_pst->rx_buffer_puc[me->data_cnt_uc++] = byte_uc;
    }
    else if (me->rx_state_uc == 2)
    {
        me->rx_state_uc = 3;
        me->ano_cfg_pst->rx_buffer_puc[me->data_cnt_uc++] = byte_uc;
    }
    else if (me->rx_state_uc == 3)
    {
        me->rx_state_uc = 4;
        me->ano_cfg_pst->rx_buffer_puc[me->data_cnt_uc++] = byte_uc;
        me->data_len_uc = byte_uc;
    }
    else if (me->rx_state_uc == 4 && me->data_len_uc > 0)
    {
        me->data_len_uc--;
        me->ano_cfg_pst->rx_buffer_puc[me->data_cnt_uc++] = byte_uc;
        if (me->data_len_uc == 0)
        {
            me->rx_state_uc = 5;
        }
    }
    else if (me->rx_state_uc == 5)
    {
        me->rx_state_uc = 6;
        me->ano_cfg_pst->rx_buffer_puc[me->data_cnt_uc++] = byte_uc;
    }
    else if (me->rx_state_uc == 6)
    {
        me->rx_state_uc = 0;
        me->ano_cfg_pst->rx_buffer_puc[me->data_cnt_uc++] = byte_uc;
        me->ano_cfg_pst->private_pst->ano_receive_anl(me->ano_cfg_pst->rx_buffer_puc,me->data_cnt_uc);
        return 1;  /* 完整帧接收完成 */
    }
    else
    {
        me->rx_state_uc = 0;
    }
    return 0;
}


static int s_frame_send(struct ano_device_t* me,uint8_t frame_num_uc)
{
    uint8_t cnt_uc = 0;
    uint8_t tx_buffer_puc[FRAME_MAX_LENGTH] = {0};

    tx_buffer_puc[cnt_uc++] = 0xAA;
    tx_buffer_puc[cnt_uc++] = me->ano_frame_pst->frame_pst[frame_num_uc].address_uc;
    tx_buffer_puc[cnt_uc++] = frame_num_uc;
    tx_buffer_puc[cnt_uc++] = 0;

    me->ano_cfg_pst->private_pst->ano_add_send_data(frame_num_uc,&cnt_uc,tx_buffer_puc);

    tx_buffer_puc[3] = cnt_uc - 4;
    

    uint8_t check_sum1_uc = 0; 
    uint8_t check_sum2_uc = 0;

    for (uint8_t i = 0; i < cnt_uc; i++) {
        check_sum1_uc += tx_buffer_puc[i];
        check_sum2_uc += check_sum1_uc;
    }

    tx_buffer_puc[cnt_uc++] = check_sum1_uc;
    tx_buffer_puc[cnt_uc++] = check_sum2_uc;

    if (me->ano_frame_pst->check_repeat_st.wait_ck_uc != 0 && frame_num_uc == 0xe0) {
        me->ano_frame_pst->send_check_st.id_uc = frame_num_uc;
        me->ano_frame_pst->send_check_st.sc_uc = check_sum1_uc;
        me->ano_frame_pst->send_check_st.ac_uc = check_sum2_uc;
    }

    me->ano_cfg_pst->private_pst->ano_send_buffer(tx_buffer_puc,cnt_uc);

    return 0;

}


static int s_send_cmd(struct ano_base_t* base ,struct command_t* cmd_pst)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    me->ano_frame_pst->send_cmd_st = *cmd_pst;
    return 0;
}

static int s_set_check_back(struct ano_base_t* base, struct check_back_t* check_back_pst)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    me->ano_frame_pst->send_check_st = *check_back_pst;
    me->ano_frame_pst->frame_pst[0x00].wts_uc = 1;                  //接受到校验位后，立刻设置wts位为1
    return 0;
}

static int s_set_par(struct ano_base_t* base, struct par_t* par_pst)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    me->ano_frame_pst->par_data_st = *par_pst;
    me->ano_frame_pst->frame_pst[0xe2].wts_uc = 1;
    return 0;
}


static int s_get_send_check_back(struct ano_base_t* base , struct check_back_t* check_back_pst)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    *check_back_pst = me->ano_frame_pst->send_check_st;
    return 0;
}

static int s_get_cmd(struct ano_base_t* base , struct command_t* cmd_pst)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    *cmd_pst = me->ano_frame_pst->send_cmd_st;
    return 0;
}

static int s_get_par(struct ano_base_t* base , struct par_t* par_pst)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    *par_pst = me->ano_frame_pst->par_data_st;
    return 0;
}

static int s_set_wts(struct ano_base_t* base,uint8_t frame_num_uc)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    me->ano_frame_pst->frame_pst[frame_num_uc].wts_uc = 1;
    return 0;
}

static int s_check_to_send(struct ano_base_t* base,uint8_t frame_num_uc)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    if (me->ano_frame_pst->frame_pst[frame_num_uc].fre_ms_us)
    {
        if(me->ano_frame_pst->frame_pst[frame_num_uc].time_cnt_ms < me->ano_frame_pst->frame_pst[frame_num_uc].fre_ms_us)
        {
            me->ano_frame_pst->frame_pst[frame_num_uc].time_cnt_ms++;
        }
        else
        {
            me->ano_frame_pst->frame_pst[frame_num_uc].time_cnt_ms = 1;
            me->ano_frame_pst->frame_pst[frame_num_uc].wts_uc = 1;
        }
    }

    if (me->ano_frame_pst->frame_pst[frame_num_uc].wts_uc)
    {
        me->ano_frame_pst->frame_pst[frame_num_uc].wts_uc = 0;
        s_frame_send(me,frame_num_uc);
    }
    return 0;

}

static int s_ck_back_check(struct ano_base_t* base)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    if (me->ano_frame_pst->check_repeat_st.wait_ck_uc == 1)
    {
        if (me->ano_frame_pst->check_repeat_st.time_dly_uc < 50)
        {
            me->ano_frame_pst->check_repeat_st.time_dly_uc++;
        }
        else
        {
            me->ano_frame_pst->check_repeat_st.time_dly_uc = 0;
            me->ano_frame_pst->check_repeat_st.repeat_cnt_uc++;
            if (me->ano_frame_pst->check_repeat_st.repeat_cnt_uc < 1)
            {
                me->ano_frame_pst->frame_pst[0xe0].wts_uc = 1;
            }
            else
            {
                me->ano_frame_pst->check_repeat_st.repeat_cnt_uc = 0;
                me->ano_frame_pst->check_repeat_st.wait_ck_uc = 0;
            }
        }
    }
    return 0;
}


static int s_set_send_id(struct ano_base_t* base,uint8_t frame_num_uc,uint16_t freq_us)
{

    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);

    me->ano_frame_pst->frame_pst[frame_num_uc].address_uc = 0xff;
    me->ano_frame_pst->frame_pst[frame_num_uc].fre_ms_us = freq_us;
    me->ano_frame_pst->frame_pst[frame_num_uc].time_cnt_ms = 0;
    return 0;
}

static int s_ano_check_data(struct ano_base_t* base)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);

    uint8_t data_puc[64] = {0};
    uint32_t read_ul = 0;

    while ((read_ul = my_ring_buf_get(*me->ano_cfg_pst->ring_buf_base_ppst,data_puc, sizeof(data_puc))) > 0) {
        for (int i = 0; i < read_ul ; i++) {
            s_data_to_ano(me,data_puc[i]);
        }
    }
    return 0;

}

static int s_clear_wait(struct ano_base_t* base)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    me->ano_frame_pst->check_repeat_st.wait_ck_uc = 0;
    return 0;
}



const ano_ops_t c_ano_normal_st = {
    .check_to_send = s_check_to_send,
    .ck_back_check = s_ck_back_check,
    .get_check_back = s_get_send_check_back,
    .get_cmd = s_get_cmd,
    .get_par = s_get_par,
    .send_cmd = s_send_cmd,
    .set_check_back = s_set_check_back,
    .set_wts = s_set_wts,
    .set_par = s_set_par,
    .set_send_id  = s_set_send_id,   
    .ano_check_data = s_ano_check_data,
    .clear_wait = s_clear_wait,
};


int ano_device_init_noraml(struct ano_device_t* me,struct ano_frame_t* ano_frame_pst,const struct ano_cfg_t* ano_cfg_pst)
{
    if (!me || !ano_frame_pst || !ano_cfg_pst->rx_buffer_puc || !ano_cfg_pst->private_pst)
        return -1;
    me->ano_frame_pst = ano_frame_pst;
    me->ano_cfg_pst = ano_cfg_pst;

    me->rx_state_uc = 0;
    me->data_cnt_uc = 0;
    me->data_len_uc = 0;
    me->base.ops = &c_ano_normal_st;                        //绑定操作表函数

    return 0;
};

