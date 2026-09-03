#include "uart_base.h"
#include "mesc.h"
#include <stdint.h>



int uart_transmit(uart_base_t* me,uint8_t* data_puc ,uint32_t len_ul)
{
    CHECKIF(!me)
    {
        return  -EINVAL;                    //  Invalid argument
    }
    return me->ops->uart_transmit(me,data_puc,len_ul);
}

int uart_receive_enable(uart_base_t* me,uint32_t time_out_ul)
{
    CHECKIF(!me)
    {
        return  -EINVAL;
    }
    return  me->ops->uart_rx_enable(me,time_out_ul);
}
