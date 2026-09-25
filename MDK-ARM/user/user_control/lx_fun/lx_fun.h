#ifndef __LX_FUN_H
#define __LX_FUN_H
#include "main.h"
#include "lx_sensor.h"
struct rt_tar_t
{
	int16_t rol;
	int16_t pit;
	int16_t thr;
	int16_t yaw_dps;
	int16_t vel_x;
	int16_t vel_y;
	int16_t vel_z;
} __attribute__((__packed__));


void battery_copy(struct lx_bat_t *out);
void pwm_out_put(void);
void rt_tar_copy(struct rt_tar_t *out);

#endif