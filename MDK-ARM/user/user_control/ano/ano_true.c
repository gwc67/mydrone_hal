#include "ano_true.h"
#include "mesc.h"
#include "FreeRTOS.h"
#include "semphr.h"
extern QueueHandle_t      ano_tx_queue;

static int s_frame_send(struct ano_base_t* base,uint8_t frame)
{
    struct ano_device_t* me = CONTAINER_OF(base, struct ano_device_t, base);
    
    uint8_t cnt = 0;
    uint8_t tx_buffer[FRAME_MAX_LENGTH] = {0};

    tx_buffer[cnt++] = 0xAA;
    tx_buffer[cnt++] = 0xFF;
    tx_buffer[cnt++] = frame;
    tx_buffer[cnt++] = 0;

    me->cfg_pst->ano_add_send_data(frame,&cnt,tx_buffer);

    tx_buffer[3] = cnt - 4;
    

    uint8_t check_sum1 = 0; 
    uint8_t check_sum2 = 0;

    for (uint8_t i = 0; i < cnt; i++) {
        check_sum1 += tx_buffer[i];
        check_sum2 += check_sum1;
    }

    tx_buffer[cnt++] = check_sum1;
    tx_buffer[cnt++] = check_sum2;

    if (me->frame_pst->check_repeat_st.wait_ck != 0 && frame == 0xe0) {
        me->frame_pst->send2check_st.id_uc = frame;
        me->frame_pst->send2check_st.sc_uc = check_sum1;
        me->frame_pst->send2check_st.ac_uc = check_sum2;
    }

    me->cfg_pst->ano_send_buffer(tx_buffer,cnt);

    return 0;

}


static int s_send_cmd(struct ano_base_t* base ,struct cmd_t* cmd_pst)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    me->frame_pst->send_cmd_st = *cmd_pst;

    struct ano_event_t event = {.frame = 0xe0,.me = base};
    xQueueSend(ano_tx_queue, &event,0);
    return 0;
}

static int s_send2check(struct ano_base_t* base, struct ck_t* ck_pst)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    me->frame_pst->send2check_st = *ck_pst;


    struct ano_event_t event = {.frame = 0x00,.me = base};
    xQueueSend(ano_tx_queue, &event,0);
    return 0;
}

static int s_set_par(struct ano_base_t* base, struct par_t* par_pst)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    me->frame_pst->par_data_st = *par_pst;
    
    struct ano_event_t event = {.frame = 0xe2,.me = base};
    xQueueSend(ano_tx_queue, &event,0);
    return 0;
}


static int s_get_send2check(struct ano_base_t* base , struct ck_t* ck_pst)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    *ck_pst = me->frame_pst->send2check_st;
    return 0;
}

static int s_get_cmd(struct ano_base_t* base , struct cmd_t* cmd_pst)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    *cmd_pst = me->frame_pst->send_cmd_st;
    return 0;
}

static int s_get_par(struct ano_base_t* base , struct par_t* par_pst)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    *par_pst = me->frame_pst->par_data_st;
    return 0;
}

static int s_data_SetWts(struct ano_base_t* base,uint8_t frame)
{
    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    struct ano_event_t event = {.frame = frame,.me = base};
    xQueueSend(ano_tx_queue, &event,10);
    return 0;
}


//对应匿名发送，使用的是订阅机制，走的是统一订阅事件
//匿名接受，使用的rx和tx单独的队列机制
static void s_ano_event_callback(enum event_id_e id,uint32_t param,void* user)
{
    xQueueSend(ano_tx_queue,(struct ano_event_t*)user,0);
}
static int s_set_send_id(struct ano_base_t* base,uint8_t frame,enum event_id_e event_id_e,uint8_t prio)
{

    struct ano_device_t *me = CONTAINER_OF(base,struct ano_device_t,base);
    event_subscribe(event_id_e, s_ano_event_callback,&me->frame_pst->ano_event_pst[frame],prio);
    return 0;
}
const ano_ops_t ano_ops_st = {
    .send_data = s_frame_send,
    .send_cmd = s_send_cmd,
    .data_SetWts = s_data_SetWts,
    .get_send2check = s_get_send2check,
    .get_cmd = s_get_cmd,
    .get_par = s_get_par,
    .set_send_id = s_set_send_id,
    .set_send2check = s_send2check,
    .set_par = s_set_par,
};

static int s_ano_rx_callback(uint8_t* data,uint32_t len32,void* user_data)
{
    struct ano_base_t* base =  (struct ano_base_t* )user_data;
    struct ano_device_t* me = CONTAINER_OF(base, struct ano_device_t, base);

    for (uint32_t i = 0;i < len32;i++ ) {
        if (me->rx_state == 0 && data[i] == 0xAA)
        {
            me->rx_state = 1;
            me->data_cnt8 = 0;
            me->data_len8 = 0;
            me->cfg_pst->rx_buffer[me->data_cnt8++] = data[i];
        }
        else if (me->rx_state == 1 && data[i] == 0xFF)
        {
            me->rx_state = 2;
            me->cfg_pst->rx_buffer[me->data_cnt8++] = data[i];
        }
        else if (me->rx_state == 2)
        {
            me->rx_state = 3;
            me->cfg_pst->rx_buffer[me->data_cnt8++] = data[i];
        }
        else if (me->rx_state == 3)
        {
            me->rx_state = 4;
            me->cfg_pst->rx_buffer[me->data_cnt8++] = data[i];
            me->data_len8 = data[i];
        }
        else if (me->rx_state == 4 && me->data_len8 > 0)
        {
            me->data_len8--;
            me->cfg_pst->rx_buffer[me->data_cnt8++] = data[i];
            if (me->data_len8 == 0)
            {
                me->rx_state = 5;
            }
        }
        else if (me->rx_state == 5)
        {
            me->rx_state = 6;
            me->cfg_pst->rx_buffer[me->data_cnt8++] = data[i];
        }
        else if (me->rx_state == 6)
        {
            me->rx_state = 0;
            me->cfg_pst->rx_buffer[me->data_cnt8++] = data[i];
            me->cfg_pst->ano_receive_anl(me->cfg_pst->rx_buffer, me->data_cnt8);
        }
        else
        {
            me->rx_state = 0;
        }
    }
    return 0;

}



int ano_device_init(struct ano_device_t* me,struct ano_frame_t* frame_pst,const struct ano_cfg_t* cfg_pst,uart_base_t* uart_base, const char* name)
{
    if (!me || !frame_pst || !cfg_pst->rx_buffer)
    {
        return -EINVAL;
    }
    me->frame_pst = frame_pst;
    me->cfg_pst = cfg_pst;
    me->data_cnt8 = 0;
    me->data_len8 = 0;
    me->rx_state = 0;

    me->base.ops = &ano_ops_st;
    me->base.name = name;
    //注册解析函数到串口
    uart_register_callback(uart_base,s_ano_rx_callback,&me->base);
    return 0;
}

