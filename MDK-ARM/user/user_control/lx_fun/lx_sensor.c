#include "lx_sensor.h"




static struct lx_vel_t s_lx_vel_fusion;  // 融合速度
static struct lx_dis_t s_lx_dis_fusion;  // 融合速度




void vel_fusion_copy(struct lx_vel_t *out)
{
    *out = s_lx_vel_fusion;
}

void dis_fusion_copy(struct lx_dis_t *out)
{
    *out = s_lx_dis_fusion;
}

