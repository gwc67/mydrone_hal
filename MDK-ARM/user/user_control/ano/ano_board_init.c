#include "ano_base.h"
#include "ano_true.h"
#include "uarts.h"
#include "driver_registry.h"
//*************************************************************************************************** */
//可不可以通过注册回调的方式是上层调用给底层，而不是在board_init中进行包含其他的上层的函数呢？
// *******************************************************************************************************

static struct ano_device_t s_com_st;
static uint8_t s_com_rxbuffer[FRAME_MAX_LENGTH];
static struct ano_frame_t s_com_frame_st;
struct ano_base_t* g_com_ano;

void ano_board_init(void)
{
    int result = 0;

    result = ano_device_init(&s_com_st, &s_com_frame_st, s_com_rxbuffer,g_uart_computer,"computer");
    if (result != 0) {
        return;
    } 
    g_com_ano = &s_com_st.base;


}

DRIVER_INIT_2(ano_board_init);