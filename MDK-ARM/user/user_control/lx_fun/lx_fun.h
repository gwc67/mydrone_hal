#ifndef __LX_FUN_H
#define __LX_FUN_H

struct lx_bat_t
{
	uint16_t voltage_100;
	uint16_t current_100;
} __attribute__((__packed__));
void battery_copy(struct lx_bat_t *out);

#endif