#ifndef __SBUS_H
#define __SBUS_H



#include "main.h"

enum
{
	CH_1_ROL=0,
	CH_2_PIT,
	CH_3_THR,
	CH_4_YAW,
	CH_5_AUX1,
	CH_6_AUX2,
	CH_7_AUX3,
	CH_8_AUX4,
	CH_9_AUX5,
	CH_10_AUX6,
	CH_NUM,
};

struct rc_ch_t
{
	int16_t channel[CH_NUM]; 
}__attribute__((__packed__)) ;

struct rc_input_t
{
	int16_t sbus_ch[16];    //原始通道信号
	struct rc_ch_t rc_ch;          //经过解析后的数据信号
	uint8_t sbus_flag;
	uint16_t signal_fre;    //1s 内的信号频率
	uint8_t no_signal;      //1： 没信号， 0 ：有信号
	uint8_t fail_safe;       
    uint8_t rc_in_mode_tmp;
};


void sbus_ch_copy(struct rc_ch_t *out);

#endif