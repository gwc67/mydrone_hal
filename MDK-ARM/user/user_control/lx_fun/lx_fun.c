#include "lx_fun.h"
#include "adc_drv.h"
#include "pwm/pwm_drv.h"
#include "ano_math.h"
//传感器采样，本就应该是定时轮询

// ####################################################################

static struct lx_bat_t s_battary;

void battery_copy(struct lx_bat_t *out)
{
    *out = s_battary;
}

void bat_sample(void)
{
    s_battary.voltage_100 =  adc_get_volatge() * 0.1 + 0.1; //发送的电压信息需要增大100倍
    s_battary.current_100 = adc_get_curr() * 125 * 0.01149;
}

// ####################################################################

void pwm_out_put(void)
{
    static int16_t ps_pwm[4];
    struct pwm_t pwm;
    //接受的pwm信号是被放大10倍的
    ps_pwm[0] = pwm_snap.pwm_m1 * 0.2f;
    ps_pwm[1] = pwm_snap.pwm_m2 * 0.2f;
    ps_pwm[2] = pwm_snap.pwm_m3 * 0.2f;
    ps_pwm[3] = pwm_snap.pwm_m4 * 0.2f;

    for (uint8_t i = 0; i < 4; i++)
    {
        ps_pwm[i] = LIMIT(ps_pwm[i],0,2000);
    }
    pwm_set_motor(ps_pwm);
}


