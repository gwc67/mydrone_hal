#ifndef __ANO_BASE_H
#define __ANO_BASE_H
#include <stdint.h>

struct check_back_t {
    uint8_t id_uc;
    uint8_t sc_uc;
    uint8_t ac_uc;
};

struct command_t{
    uint8_t cid_uc;
    uint8_t cmd_puc[10];
};

struct par_t{
    uint16_t par_id_us;
    int32_t par_val_l;
};

typedef struct ano_base_t ano_base_t;


typedef struct
{
    int (*send_cmd)(ano_base_t*me, struct command_t* cmd_pst);
    int (*set_check_back)(ano_base_t*me, struct check_back_t* check_back_pst);
    int (*set_par)(ano_base_t*me, struct par_t* par_pst);
    int (*set_send_id)(ano_base_t*me,uint8_t frame_uc,uint16_t freq_us);
    int (*check_to_send)(ano_base_t* me,uint8_t frame_uc);
    int (*set_wts)(ano_base_t* me,uint8_t frame_uc);
    int (*get_check_back)(ano_base_t *me,struct check_back_t* check_back_pst);
    int (*get_cmd)(ano_base_t *me,struct command_t* cmd_pst);
    int (*get_par)(ano_base_t *me,struct par_t* par_pst);
    int (*ck_back_check)(ano_base_t*me);
    int (*ano_check_data)(ano_base_t*me);
    int (*clear_wait)(ano_base_t* me);
    // int (*send_string)(ano_base_t *me, int32_t lValue, char *pcstr);
    // int (*ano_check_data)(ano_base_t *me,stUartBase* pstbase_uart);
    // int (*ano_printf)(ano_base_t *me, ano_printf_color_em color_em, const char *fmt, va_list ap);
    // int8_t_t (*wait_ck_get_c)(ano_base_t* me);
}ano_ops_t;

struct ano_base_t
{
   const ano_ops_t* ops;
};

int ano_send_cmd(struct ano_base_t*me, struct command_t* cmd_pst);
int ano_set_check_back(struct ano_base_t*me, struct check_back_t* check_back_pst);
int ano_set_par(struct ano_base_t*me, struct par_t* par_pst);
int ano_set_send_id(struct ano_base_t*me,uint8_t frame_uc,uint16_t freq_us);
int ano_check_to_send(struct ano_base_t* me,uint8_t frame_uc);
int ano_set_wts(struct ano_base_t* me,uint8_t frame_uc);
int ano_get_check_back(struct ano_base_t *me,struct check_back_t* check_back_pst);
int ano_get_cmd(struct ano_base_t *me,struct command_t* cmd_pst);
int ano_get_par(struct ano_base_t *me,struct par_t* par_pst);
int ano_ck_back_check(struct ano_base_t*me);
int ano_check_data(struct ano_base_t*me);
int ano_clear_wait(ano_base_t* me);






#endif
