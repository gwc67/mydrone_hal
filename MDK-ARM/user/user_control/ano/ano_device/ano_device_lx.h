#ifndef __ANO_DEVICE_LX_H
#define __ANO_DEVICE_LX_H

#include "main.h"

struct lx_qua_t
{
	int16_t w_x10000;
	int16_t x_x10000;
	int16_t y_x10000;
	int16_t z_x10000;
	uint8_t state;
} __attribute__((__packed__));

// uint8_t fc_mode_sta;
// uint8_t unlock_sta;
struct lx_state_t
{
	//模式
	uint8_t fc_mode_cmd;
	//解锁上锁
	uint8_t unlock_cmd;
	//指令功能
	uint8_t CID;
	uint8_t CMD_0;
	uint8_t CMD_1;
} ;

struct lx_vel_t
{
	int16_t vel_x;
	int16_t vel_y;
	int16_t vel_z;
} __attribute__((__packed__));

struct lx_led_t
{
	uint8_t red;
	uint8_t blue;
	uint8_t green;
	uint8_t ano;
}

void lx_qua_copy(struct lx_qua_t *out);

#endif