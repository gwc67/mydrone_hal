#include "ano.h"
#include "ano_device_lx.h"
#include "driver_registry.h"
#include "uarts.h"
#define LX_BAT_TX      0x0d
#define LX_GPS_TX      0x30
#define LX_GEN_VEL_TX  0x33
#define LX_GEN_DIS_TX  0x34
#define LX_RC_CH_TX    0x40
#define LX_RT_TAR_TX   0x41
#define LX_CMD_TX      0xe0
#define LX_PAR_TX      0xe2


#define LX_QUA_RX      0x04
#define LX_STATE_RX    0x06
#define LX_FC_VEL_RX   0x07
#define LX_LED_RX      0x0f
#define LX_PWM_RX      0x20

static struct lx_qua_t   s_lx_qua;
static struct lx_state_t s_lx_state
static struct lx_vel_t   s_lx_vel; 
static struct lx_led_t   s_led_light;

#define ANO_HANDEL  g_lx_ano

void lx_add_send_data(uint8_t frame,uint8_t *cnt_ptr,uint8_t* data)
{
     
}

void lx_receive_anl(uint8_t* data,uint8_t len8)
{
    uint8_t check_sum1 = 0, check_sum2 = 0;

    if (*(data + 3) != (len8 - 6))
        return;

    for (uint8_t i = 0; i < len8 - 2; i++)
    {
        check_sum1 += *(data + i);
        check_sum2 += check_sum1;
    }

    if ((check_sum1 != *(data + len8 - 2)) || (check_sum2 != *(data + len8 - 1)))
        return;

    if (*(data) != 0xAA || (*(data + 1) != 0xFF))
        return;

    if (*(data + 2) == LX_QUA_RX) {
        memcpy(&s_lx_qua,(data + 4),sizeof(s_lx_qua));
    }
    else if (*(data + 2) == LX_STATE_RX) {
        memcpy(&s_lx_state,(data + 4),sizeof(s_lx_state));
    }
    else if (*(data + 2) == LX_FC_VEL_RX) {
        memcpy(&s_lx_vel,(data + 4),sizeof(s_lx_vel));
    }
    else if (*(data + 2) == LX_FC_VEL_RX) {
        memcpy(&s_lx_vel,(data + 4),sizeof(s_lx_vel));
    }
    else if (*(data + 2) == LX_LED_RX) {
        memcpy(&s_led_light,(data + 4),sizeof(s_led_light));
    }
    else if (*(data + 2) == 0xe0)
    {
        struct ck_t send2check = {0};
        send2check.id_uc = *(data + 2);
        send2check.sc_uc = check_sum1;
        send2check.ac_uc = check_sum2;
        ano_set_send2check(ANO_HANDEL,&send2check);
    }
    else if (*(data + 2) == 0xe1)
    {
        struct ck_t send2check = {0};
        send2check.id_uc = *(data + 2);
        send2check.sc_uc = check_sum1;
        send2check.ac_uc = check_sum2;
        ano_set_send2check(g_com_ano,&send2check);
    }
    if(*(data + 2) == 0x00)
    {
        struct ck_t snap = {0};
        //如何实现一次性发送到之后每隔100ms呢？
        //收到之后，应该取消某个东西
        //发送一个0xe0命令帧后，要给个响应，否则会每隔100ms发送5次，5次过后不发
        //0xe0 后触发一个定时器事件，每次触发一次tick加一，直到tick = 5；那么freertos怎么做到呢？
        if (snap.id_uc == *(data + 4) && snap.sc_uc == *(data + 5) && snap.ac_uc == *(data + 6)) {
            // ano_clear_wait(g_com_ano_pst);
        }
    }
}

// uart_transmit(g_uart_com, data, len8);


void lx_send_buffer(uint8_t* data,uint8_t len8)
{
    
}

static void s_ano_device_lx_init(void)
{
    ano_register_callback(ANO_HANDEL, lx_receive_anl, lx_add_send_data, lx_send_buffer);
    
    ano_set_send_id(ANO_HANDEL, 0x02,EVT_TIMER_1000MS,2);
    
    ano_set_send_id(ANO_HANDEL, 0x01,EVT_TIMER_500MS,1);
}

DRIVER_INIT_3(s_ano_device_lx_init);