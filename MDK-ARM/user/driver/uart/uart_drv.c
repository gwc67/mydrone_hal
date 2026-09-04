#include "uarts.h"
#include "main.h"

// void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
// {
//     if (huart == uart_handle_get(pstbase_subus_uart)) //?????
//     {
//         HAL_UART_AbortReceive_IT(uart_handle_get(pstbase_subus_uart));
//         uart_receive_enable(pstbase_subus_uart);
//     }
//     if (huart == uart_handle_get(pstbase_lx_uart)) //????imu
//     {
//         //清除错误标志位
//         __HAL_UART_CLEAR_FLAG(uart_handle_get(pstbase_lx_uart), UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_PEF | UART_CLEAR_FEF);
//         HAL_UART_AbortReceive_IT(uart_handle_get(pstbase_lx_uart));
//         uart_receive_enable(pstbase_lx_uart);
//     }
//     if (huart == uart_handle_get(pstbase_anoof_uart)) //????
//     {
//         __HAL_UART_CLEAR_FLAG(uart_handle_get(pstbase_anoof_uart), UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_PEF | UART_CLEAR_FEF);
//         HAL_UART_AbortReceive_IT(uart_handle_get(pstbase_anoof_uart));     
//         uart_receive_enable(pstbase_anoof_uart);

//     }
// #if COM_DEBUG    
//     if (huart == uart_handle_get(pstbase_com_uart))
//     {
//         HAL_UART_AbortReceive_IT(uart_handle_get(pstbase_com_uart));
//         uart_receive_enable(pstbase_com_uart);
//     }
//     #endif
//     if (huart == uart_handle_get(pstbase_ground_uart))
//     {
//         HAL_UART_AbortReceive_IT(uart_handle_get(pstbase_ground_uart));
//         uart_receive_enable(pstbase_ground_uart);
//     }
//     if (huart == uart_handle_get(pstbase_jesnano_uart))
//     {
//         __HAL_UART_CLEAR_FLAG(uart_handle_get(pstbase_jesnano_uart), UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_PEF | UART_CLEAR_FEF);
//         HAL_UART_AbortReceive_IT(uart_handle_get(pstbase_jesnano_uart));
//         uart_receive_enable(pstbase_jesnano_uart);
//     }
//     if (huart == uart_handle_get(pstbase_usart5_uart)) // GPS
//     {
//         __HAL_UART_CLEAR_FLAG(uart_handle_get(pstbase_usart5_uart), UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_PEF | UART_CLEAR_FEF);
//         HAL_UART_AbortReceive_IT(uart_handle_get(pstbase_usart5_uart));
//         uart_receive_enable(pstbase_usart5_uart);
//     }
// }


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart == uart_get_handle(g_uart_computer)) {
        uart_rx_isr(g_uart_computer, Size);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == uart_get_handle(g_uart_computer)) {
        uart_tx_isr(g_uart_computer);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    
}
