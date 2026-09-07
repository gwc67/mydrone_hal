#include "ano_device_com.h"
#include "ano.h"
#include "uarts.h"
#include "driver_registry.h"

static void s_ano_device_com_init(void)
{
    ano_set_send_id(g_com_ano, 0x01,EVT_TIMER_1000MS,1);
}
DRIVER_INIT_3(s_ano_device_com_init);

void com_receive_anl(uint8_t* data,uint8_t len8)
{
    uint8_t check_sum1 = 0, check_sum2 = 0;
    if (*(data + 3) != (len8 - 6))
    {
        return;
    }
    for (uint8_t i = 0; i < len8 - 2; i++)
    {
        check_sum1 += *(data + i);
        check_sum2 += check_sum1;
    }

    if ((check_sum1 != *(data + len8 - 2)) || (check_sum2 != *(data + len8 - 1)))
    {
        return;
    }
    
    if(*(data + 2) == 0x00)
    {
        // struct ck_t snap = {0};
        // if (snap.id_uc == *(data + 4) && snap.sc_uc == *(data + 5) && snap.ac_uc == *(data + 6)) {
            // ano_clear_wait(g_com_ano_pst);
        // }
        // data
    }
    else if (*(data + 2) == 0xe0)
    {

        // struct check_back_t check_back_st = {0};
        // check_back_st.id_uc = *(data + 2);
        // check_back_st.sc_uc = check_sum1;
        // check_back_st.ac_uc = check_sum2;
        // ano_set_check_back(g_com_ano_pst,&check_back_st);
    }
    // else if(*(data + 2) == SIMULINK_PID_RX)
    // {
    //     // gpio_pin_toggle_dt(&led0);
    //     // BALANCE_KP = *(int32_t*)(data + 4) / 1000.0;
    //     BALANCE_KD = *(int32_t*)(data + 12) / 1000.0;
    // }

}

void com_add_send_data(uint8_t frame,uint8_t *cnt_ptr,uint8_t* data)
{
    switch (frame) {
        case 0x00:
        {
            // struct check_back_t snap = {0};
            // ano_get_check_back(g_com_ano_pst, &snap);
            // memcpy(data + *cnt_puc, &snap, sizeof(snap));
            // *cnt_puc += sizeof(snap);
            data[(*cnt_ptr)++] = 0x01;
        }
        break;
        case 0x01:
        {
            data[(*cnt_ptr)++] = 0x01;
        }
        break;
        default:
            break;
    }
}


void com_send_buffer(uint8_t *data,uint8_t len8)
{
    uart_transmit(g_uart_computer, data, len8);
}
