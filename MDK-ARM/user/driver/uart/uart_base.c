#include "uart_base.h"
#include "mesc.h"
#include <stdint.h>



int uart_transmit(uart_base_t* me,uint8_t* data ,uint32_t len32)
{
    CHECKIF(!me || !me->ops->uart_transmit)
    {
        return  -EINVAL;                    //  Invalid argument
    }
    
    return me->ops->uart_transmit(me,data,len32);
}

int uart_receive_enable(uart_base_t* me)
{
    CHECKIF(!me || !me->ops->uart_rx_enable)
    {
        return  -EINVAL;
    }
    return  me->ops->uart_rx_enable(me);
}
int uart_rx_isr(uart_base_t* me,uint32_t len32)
{
    CHECKIF(!me || !me->ops->uart_rx_isr)
    {
        return  -EINVAL;
    }
    return  me->ops->uart_rx_isr(me,len32);
}

UART_HandleTypeDef* uart_get_handle(uart_base_t* me)
{
    CHECKIF(!me || !me->ops->uart_get_handle)
    {
        return NULL;
    }
    return  me->ops->uart_get_handle(me);
}
int uart_register_callback(uart_base_t* me,uart_callback_t callback,void* user_data)
{
    CHECKIF(!me || !me->ops->uart_register_callback)
    {
        return -EINVAL;
    }
    return  me->ops->uart_register_callback(me,callback,user_data);
}

int uart_rx_analyze(uart_base_t*me)
{
    CHECKIF(!me || !me->ops->uart_rx_analyze)
    {
        return -EINVAL;
    }
    return  me->ops->uart_rx_analyze(me);
}