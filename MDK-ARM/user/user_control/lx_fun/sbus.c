#include "sbus.h"
#include "driver_registry.h"
#include "uarts.h"
static struct rc_input_t s_rc_input;


#define SBUS_FRAME_LEN 25
#define SBUS_CH_NUM    16

static inline int16_t sbus_channel(const uint8_t *data,uint8_t channel)
{
    uint16_t bit_pos = channel * 11; 
    uint8_t byte = bit_pos / 8; //第几个字节前面
    uint8_t bit   = bit_pos % 8; //该字节的第几bit;
    uint32_t val  = data[byte] | (data[byte + 1] << 8) | (data[byte + 2] << 16);  //合并3个相邻byte
    return  (int16_t)((val >> bit) & 0x7FF);                                      //先偏移bit 再取第11位 0x7FF即11位 
}

// 不过说实话，这种写法并不是 SBUS 协议的标准做法，更像是匿名为了兼容自家不同版本接收机而做的“土办法”。标准 Futaba SBUS 就是固定 0x00 帧尾，不需要 frame_cnt。

// typedef  int (*uart_callback_t)(uint8_t* data,uint32_t len32,void* user_data);
static void sbus_frame_decode(const uint8_t *frame)
{
    for (int i = 0; i < SBUS_CH_NUM; i++)
    {
        s_rc_input.sbus_ch[i] = sbus_channel(frame + 1,i); // +1?
    }
    s_rc_input.sbus_flag = frame[23];
    if (!(s_rc_input.sbus_flag & 0x08)) // 不是失控态 每解析一次 说明解析计数加1
    {
        s_rc_input.signal_fre ++;
        s_rc_input.rc_in_mode_tmp = 2;// rc_in 里面输入的模式
    }
}


int sbus_analyze(uint8_t* data,uint32_t len,void* user_data)
{
    static uint8_t s_sbus_frame[SBUS_FRAME_LEN];
    static uint8_t s_frame_cnt;
    
    for (int i = 0; i < len; i ++) {
        if (s_frame_cnt == 0 && data[i] != 0x0F) {
            continue;
        }
        s_sbus_frame[s_frame_cnt++] = data[i];
        if (s_frame_cnt == SBUS_FRAME_LEN) {
            s_frame_cnt = 0;
            if (s_sbus_frame[24] == 0x00)
            {
                sbus_frame_decode(s_sbus_frame);
            }
        }
    }
    return 0;
}

//1s执行一次的函数




void subs_init(void)
{
    uart_register_callback(g_uart_sbus,sbus_analyze, NULL);
    s_rc_input.fail_safe = 1;// no_signal：是飞控自己统计出来的（1 秒内成功帧数 ≤ 5），属于“软件层面的失联判断”。
    s_rc_input.no_signal = 1;// fail_safe：是接收机硬件层面告知飞控的（通过 SBUS 帧内的标志位），属于“硬件层面的失控通知”。
}
DRIVER_INIT_3(subs_init);





static void sbus_signalcheck(void)
{
    static uint16_t s_time_dly;
    s_time_dly += 10;                                       // 1 对应1ms 是由 lx_task的运行频率所决定的
    if (s_time_dly > 1000)
    {
        s_time_dly = 0;
        // 记录这1秒的帧数
        if (s_rc_input.signal_fre <= 5)
        {
            s_rc_input.no_signal = 1;                       // 帧数太少 → 无信号 是no_signal
        }
        else
        {
            s_rc_input.no_signal = 0;                       // 帧数够多 → 有信号
        }
        s_rc_input.signal_fre = 0;                                
    }
}

void sbus_ch_copy(struct rc_ch_t *out)
{
    *out = s_rc_input.rc_ch;
}

