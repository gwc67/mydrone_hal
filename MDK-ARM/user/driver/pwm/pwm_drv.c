/******************** (C) COPYRIGHT 2017 ANO Tech ********************************
 * 作者    ：匿名科创
 * 官网    ：www.anotc.com
 * 淘宝    ：anotc.taobao.com
 * 技术Q群 ：190169595
 * 描述    ：PWM输出
**********************************************************************************/
#include "pwm_drv.h"
#include "main.h"
#include "tim.h"
#include "driver_registry.h"
static uint16_t prepareDshotPacket(uint16_t value)
{
	uint16_t packet = (value << 1) | 0;//不打开遥测
    // 将12位数据分为3组 每组4位, 进行异或
    // compute checksum
    int csum = 0;
    int csum_data = packet;
    for (int i = 0; i < 3; i++) {
        csum ^=  csum_data;   // xor data by nibbles
        csum_data >>= 4;
    }
    //取最后四位 其他的不要 
    csum &= 0xf;
    // append checksum 将CRC添加到后四位
    packet = (packet << 4) | csum;
    return packet;
}

//打包dshot数据包，value的取值范围是0-1999
#define ESC_BIT_0 299
#define ESC_BIT_1 599
static void pwmWriteDigital(int16_t value, uint32_t *pwmData)
{
	value += 48;
	//转化成48 - 2047范围
	if(value >= 1999) value = 1999;
	if(value <= 0) value = 0;
	
	//打包好要发送的pwm数据
	value = (uint16_t) value;
	value = prepareDshotPacket(value);//不开启遥测
	
	pwmData[0] = 0; 
    pwmData[1]  = (value & 0x8000) ? ESC_BIT_1 : ESC_BIT_0;
    pwmData[2]  = (value & 0x4000) ? ESC_BIT_1 : ESC_BIT_0;
    pwmData[3]  = (value & 0x2000) ? ESC_BIT_1 : ESC_BIT_0;
    pwmData[4]  = (value & 0x1000) ? ESC_BIT_1 : ESC_BIT_0;
    pwmData[5]  = (value & 0x0800) ? ESC_BIT_1 : ESC_BIT_0;
    pwmData[6]  = (value & 0x0400) ? ESC_BIT_1 : ESC_BIT_0;
    pwmData[7]  = (value & 0x0200) ? ESC_BIT_1 : ESC_BIT_0;
    pwmData[8]  = (value & 0x0100) ? ESC_BIT_1 : ESC_BIT_0;
    pwmData[9]  = (value & 0x0080) ? ESC_BIT_1 : ESC_BIT_0;
    pwmData[10] = (value & 0x0040) ? ESC_BIT_1 : ESC_BIT_0;
    pwmData[11] = (value & 0x0020) ? ESC_BIT_1 : ESC_BIT_0;
    pwmData[12] = (value & 0x0010) ? ESC_BIT_1 : ESC_BIT_0; 	
    pwmData[13] = (value & 0x0008) ? ESC_BIT_1 : ESC_BIT_0;
    pwmData[14] = (value & 0x0004) ? ESC_BIT_1 : ESC_BIT_0;
    pwmData[15] = (value & 0x0002) ? ESC_BIT_1 : ESC_BIT_0;
    pwmData[16] = (value & 0x0001) ? ESC_BIT_1 : ESC_BIT_0;
	
}


//电调的初始化
void pwm_init(void)
{

	int16_t PWM[4] = {0};
	for(uint16_t i = 0; i < 1000; i ++)
	{
        //这里严格是两秒种
        osDelay(1);
		pwm_set_motor(PWM);
	}
}

DRIVER_INIT_1(pwm_init);

//输出dshot信号给电调
#define pwm_NU 18 // 扩大倍数应该

void pwm_set_motor(int16_t pwm[])
{	
    //定时器 or GPIO示波器 测运行时间
	static __attribute__((section(".dma_buf")))  uint32_t M1_pwmData[pwm_NU * 2]={0};
	static __attribute__((section(".dma_buf")))  uint32_t M2_pwmData[pwm_NU * 2]={0};
	static __attribute__((section(".dma_buf")))  uint32_t M3_pwmData[pwm_NU * 2]={0};
	static __attribute__((section(".dma_buf")))  uint32_t M4_pwmData[pwm_NU * 2]={0};
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
	pwmWriteDigital(pwm[0], M1_pwmData);
	HAL_TIM_PWM_Start_DMA(&htim5, TIM_CHANNEL_3, M1_pwmData, pwm_NU * 2);//注意定时器通道和电机顺序是调换的
	pwmWriteDigital(pwm[1], M2_pwmData);
	HAL_TIM_PWM_Start_DMA(&htim5, TIM_CHANNEL_1, M2_pwmData, pwm_NU * 2);
	pwmWriteDigital(pwm[2], M3_pwmData);
	HAL_TIM_PWM_Start_DMA(&htim5, TIM_CHANNEL_2, M3_pwmData, pwm_NU * 2);
	pwmWriteDigital(pwm[3], M4_pwmData);
	HAL_TIM_PWM_Start_DMA(&htim5, TIM_CHANNEL_4, M4_pwmData, pwm_NU * 2);
}

/******************* (C) COPYRIGHT 2014 ANO TECH *****END OF FILE************/
