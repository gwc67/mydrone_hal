#include "ano_true.h"
#include "mesc.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "event.h"
extern QueueHandle_t      ano_tx_queue;

static int s_frame_send(struct ano_base_t* base,uint8_t frame)
{
    struct ano_device_t* me = CONTAINER_OF(base, struct ano_device_t, base);
    
    uint8_t cnt = 0;
    uint8_t tx_buffer[FRAME_MAX_LENGTH] = {0};

    tx_buffer[cnt++] = 0xAA;
    tx_buffer[cnt++] = me->ano_frame_pst->frame_pst[frame].address;
    tx_buffer[cnt++] = frame;
    tx_buffer[cnt++] = 0;

    me->ano_cfg_pst->ano_add_send_data(frame,&cnt,tx_buffer);

    tx_buffer[3] = cnt - 4;
    

    uint8_t check_sum1 = 0; 
    uint8_t check_sum2 = 0;

    for (uint8_t i = 0; i < cnt; i++) {
        check_sum1 += tx_buffer[i];
        check_sum2 += check_sum1;
    }

    tx_buffer[cnt++] = check_sum1;
    tx_buffer[cnt++] = check_sum2;

    if (me->ano_frame_pst->check_repeat_st.wait_ck != 0 && frame == 0xe0) {
        me->ano_frame_pst->send2check_st.id_uc = frame;
        me->ano_frame_pst->send2check_st.sc_uc = check_sum1;
        me->ano_frame_pst->send2check_st.ac_uc = check_sum2;
    }

    me->ano_cfg_pst->ano_send_buffer(tx_buffer,cnt);

    return 0;

}


static int s_send_cmd(struct ano_base_t* base ,struct cmd_t* cmd_pst)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    me->ano_frame_pst->send_cmd_st = *cmd_pst;

    struct ano_event_t event = {.frame = 0xe0,.me = base};
    xQueueSend(ano_tx_queue, &event,0);
    return 0;
}

static int s_send2check(struct ano_base_t* base, struct check_back_t* ck_pst)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    me->ano_frame_pst->send2check_st = *ck_pst;


    struct ano_event_t event = {.frame = 0x00,.me = base};
    xQueueSend(ano_tx_queue, &event,0);
    return 0;
}

static int s_set_par(struct ano_base_t* base, struct par_t* par_pst)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    me->ano_frame_pst->par_data_st = *par_pst;
    
    struct ano_event_t event = {.frame = 0xe2,.me = base};
    xQueueSend(ano_tx_queue, &event,0);
    return 0;
}


static int s_get_send2check(struct ano_base_t* base , struct check_back_t* ck_pst)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    *ck_pst = me->ano_frame_pst->send2check_st;
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

static int s_data_SetWts(struct ano_base_t* base,uint8_t frame)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    struct ano_event_t event = {.frame = frame,.me = base};
    xQueueSend(ano_tx_queue, &event,10);
    return 0;
}

static void s_ano_event_callback(enum event_id_e id,uint32_t param,void* user)
{
    xQueueSend(ano_tx_queue,(struct ano_event_t*)user,0);
}

static int s_set_send_id(struct ano_base_t* base,uint8_t frame,enum event_id_e event_id_e,uint8_t prio)
{

    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    event_subscribe(event_id_e, s_ano_event_callback,&me->ano_frame_pst->ano_event_pst[frame],prio);
    return 0;
}

// static int s_clear_wait(struct ano_base_t* base)
// {
//     struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
//     me->ano_frame_pst->check_repeat_st.wait_ck_uc = 0;
//     return 0;
// }


const ano_ops_t ano_ops_st = {
    .send_data = s_frame_send,
    .send_cmd = s_send_cmd,
    .send_data_callback = s_data_SetWts,
    .get_send2check = s_get_send2check,
};

static int s_ano_rx_callback(uint8_t* data,uint32_t len32,void* user_data)
{
    struct ano_base_t* base =  (struct ano_base_t* )user_data;
    struct ano_device_t* me = CONTAINER_OF(base, struct ano_device_t, base);

    for (uint32_t i = 0;i++; i < len32) {
        if (me->rx_state_uc == 0 && data[i] == 0xAA)
        {
            me->rx_state_uc = 1;
            me->data_cnt_uc = 0;
            me->data_len_uc = 0;
            me->ano_cfg_pst->rx_buffer_puc[me->data_cnt_uc++] = data[i];
        }
        else if (me->rx_state_uc == 1 && data[i] == 0xFF)
        {
            me->rx_state_uc = 2;
            me->ano_cfg_pst->rx_buffer_puc[me->data_cnt_uc++] = data[i];
        }
        else if (me->rx_state_uc == 2)
        {
            me->rx_state_uc = 3;
            me->ano_cfg_pst->rx_buffer_puc[me->data_cnt_uc++] = data[i];
        }
        else if (me->rx_state_uc == 3)
        {
            me->rx_state_uc = 4;
            me->ano_cfg_pst->rx_buffer_puc[me->data_cnt_uc++] = data[i];
            me->data_len_uc = data[i];
        }
        else if (me->rx_state_uc == 4 && me->data_len_uc > 0)
        {
            me->data_len_uc--;
            me->ano_cfg_pst->rx_buffer_puc[me->data_cnt_uc++] = data[i];
            if (me->data_len_uc == 0)
            {
                me->rx_state_uc = 5;
            }
        }
        else if (me->rx_state_uc == 5)
        {
            me->rx_state_uc = 6;
            me->ano_cfg_pst->rx_buffer_puc[me->data_cnt_uc++] = data[i];
        }
        else if (me->rx_state_uc == 6)
        {
            me->rx_state_uc = 0;
            me->ano_cfg_pst->rx_buffer_puc[me->data_cnt_uc++] = data[i];
            me->ano_cfg_pst->ano_receive_anl(me->ano_cfg_pst->rx_buffer_puc, me->data_cnt_uc);
        }
        else
        {
            me->rx_state_uc = 0;
        }
    }
    return 0;

}
// int ano_device_init_noraml(struct ano_device_t* me,struct ano_frame_t* ano_frame_pst,const struct ano_cfg_t* ano_cfg_pst)
// {
//     if (!me || !ano_frame_pst || !ano_cfg_pst->rx_buffer_puc || !ano_cfg_pst->private_pst)
//         return -1;
//     me->ano_frame_pst = ano_frame_pst;
//     me->ano_cfg_pst = ano_cfg_pst;

//     me->rx_state_uc = 0;
//     me->data_cnt_uc = 0;
//     me->data_len_uc = 0;
//     me->base.ops = &c_ano_normal_st;                        //绑定操作表函数

//     return 0;
// };

