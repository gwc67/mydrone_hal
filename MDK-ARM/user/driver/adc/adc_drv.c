#include "adc_drv.h"
#include "adc.h"
#include "driver_registry.h"
#define  ADC_VALUE_LENGTH 50

#define UP_R 10 //10K
#define DW_R 1	//1K

__attribute__((section(".dma_buf"))) static  uint16_t  voltage_adc[ADC_VALUE_LENGTH] = {0};
__attribute__((section(".dma_buf"))) static uint16_t curr_adc[ADC_VALUE_LENGTH] = {0};

void adc_init(void)
{
	HAL_ADC_Stop(&hadc1);
	HAL_ADC_Stop(&hadc2);
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    // HAL_Delay(10);
    osDelay(10);

    HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    osDelay(10);

    HAL_ADC_Start_DMA(&hadc2, (uint32_t *)voltage_adc, ADC_VALUE_LENGTH);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)curr_adc, ADC_VALUE_LENGTH);
}
DRIVER_INIT_1(adc_init);


// tmp / 65535	把 ADC 原始值归一化成 0~1 的比例
// × 3300	换算成 ADC 引脚上的电压（mV）
double adc_get_volatge(void)
{
	double tmp = 0;
	for (uint8_t i = 0; i < ADC_VALUE_LENGTH; i++)
	{
		tmp += voltage_adc[i] * 0.02;    //取平均
	}
	return tmp / 65535 * 3300 * (UP_R + DW_R) / DW_R ; 
}

double adc_get_curr(void)
{
	double tmp = 0;
	for (uint8_t i = 0; i < ADC_VALUE_LENGTH; i++)
	{
		tmp += curr_adc[i] * 0.02f;     //取平均
	}
	return tmp / 65535 * 3270; 
}
