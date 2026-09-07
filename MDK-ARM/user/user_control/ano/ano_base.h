#ifndef __ANO_BASE_H
#define __ANO_BASE_H
#include <stdint.h>

#include "event.h"
struct ck_t {
    uint8_t id_uc;
    uint8_t sc_uc;
    uint8_t ac_uc;
};

struct cmd_t{
    uint8_t cid_uc;
    uint8_t cmd_puc[10];
};

struct par_t{
    uint16_t par_id_us;
    int32_t par_val_l;
};

typedef struct ano_base_t ano_base_t;

struct ano_event_t
{
    struct ano_base_t* me;
    uint8_t frame;   
};


typedef void (*ano_receive_anl_t)(uint8_t* data,uint8_t len8);
typedef void (*ano_add_send_data_t)(uint8_t frame,uint8_t *cnt_ptr,uint8_t* data);
typedef void (*ano_send_buffer_t)(uint8_t *data,uint8_t len8);    //不同设备的指针对应的特殊指针



typedef struct
{
    int (*send_cmd)(ano_base_t*me, struct cmd_t* cmd_pst);
    int (*set_send2check)(ano_base_t*me, struct ck_t* ck_pst);
    int (*set_par)(ano_base_t*me, struct par_t* par_pst);
    int (*set_send_id)(struct ano_base_t* base,uint8_t frame,enum event_id_e event_id_e,uint8_t prio);
    int (*send_data)(ano_base_t* me, uint8_t frame); // 发送数据的函数
    int (*data_SetWts)(struct ano_base_t* me,uint8_t frame); //触发发送数据的函数，内部为唤醒对应线程的函数
    int (*get_send2check)(struct ano_base_t* me , struct ck_t* ck_pst);
    int (*get_cmd)(ano_base_t *me,struct cmd_t* cmd_pst);
    int (*get_par)(ano_base_t *me,struct par_t* par_pst);
    int (*clear_wait)(ano_base_t* me);
    int (*register_callback)(ano_base_t* me,ano_receive_anl_t receive_anl,ano_add_send_data_t add_send_data,ano_send_buffer_t send_buffer);
    // int (*ck_back_check)(ano_base_t*me);
    // int (*ano_check_data)(ano_base_t*me);
}ano_ops_t;

struct ano_base_t
{
    const char* name;
    const ano_ops_t* ops;
};

int ano_send_cmd(struct ano_base_t*me, struct cmd_t* cmd_pst);
int ano_set_send2check(struct ano_base_t*me, struct ck_t* ck_pst);
int ano_set_par(struct ano_base_t*me, struct par_t* par_pst);
int ano_set_send_id(struct ano_base_t* base,uint8_t frame,enum event_id_e event_id_e,uint8_t prio);
int ano_send_data(ano_base_t* me, uint8_t frame);
int ano_data_setWts(struct ano_base_t* base,uint8_t frame);
int ano_get_send2check(struct ano_base_t* base , struct ck_t* ck_pst);
int ano_get_cmd(ano_base_t *me,struct cmd_t* cmd_pst);
int ano_get_par(ano_base_t *me,struct par_t* par_pst);
int ano_clear_wait(ano_base_t* me);
int ano_register_callback(ano_base_t* me,ano_receive_anl_t receive_anl,ano_add_send_data_t add_send_data,ano_send_buffer_t send_buffer);

// int ano_check_to_send(struct ano_base_t* me,uint8_t frame_uc);

// int ano_set_wts(struct ano_base_t* me,uint8_t frame_uc);
// int ano_get_check_back(struct ano_base_t *me,struct ck_t* check_back_pst);
// int ano_get_cmd(struct ano_base_t *me,struct cmd_t* cmd_pst);
// int ano_get_par(struct ano_base_t *me,struct par_t* par_pst);
// int ano_ck_back_check(struct ano_base_t*me);
// int ano_check_data(struct ano_base_t*me);
// int ano_clear_wait(ano_base_t* me);






#endif
