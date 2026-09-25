#ifndef __LX_SENSOR_H
#define __LX_SENSOR_H

#include "main.h"

struct lx_vel_t
{
	int16_t vel_x;
	int16_t vel_y;
	int16_t vel_z;
} __attribute__((__packed__));

struct lx_dis_t
{
    uint8_t direction;
    uint16_t angle_100;
    int32_t distance_cm;
} __attribute__((__packed__));

struct lx_bat_t
{
	uint16_t voltage_100;
	uint16_t current_100;
} __attribute__((__packed__));


void vel_fusion_copy(struct lx_vel_t *out);

void dis_fusion_copy(struct lx_dis_t *out);

#endif