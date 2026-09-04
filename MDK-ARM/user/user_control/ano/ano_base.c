#include "ano_base.h"
#include "stdio.h"
#include "mesc.h"
int ano_send_cmd(ano_base_t*me, struct command_t* cmd_pst)
{
    CHECKIF(me == NULL || me->ops == NULL || me->ops->send_cmd == NULL ) {
        return  -EINVAL;                    //  Invalid argument
    }
    return me->ops->send_cmd(me,cmd_pst);
}

int ano_set_check_back(ano_base_t*me, struct check_back_t* check_back_pst)
{
    CHECKIF(me == NULL || me->ops == NULL || me->ops->set_check_back == NULL ) {
        return  -EINVAL;                    //  Invalid argument
    }
    return me->ops->set_check_back(me, check_back_pst);
}

int ano_set_par(ano_base_t*me, struct par_t* par_pst)
{
    CHECKIF(me == NULL || me->ops == NULL || me->ops->set_par == NULL ) {
        return  -EINVAL;                    //  Invalid argument
    }
    return me->ops->set_par(me, par_pst);
}

int ano_set_send_id(ano_base_t*me,uint8_t frame_num_c,uint16_t freq_us)
{
    CHECKIF(me == NULL || me->ops == NULL || me->ops->set_send_id == NULL ) {
        return  -EINVAL;                    //  Invalid argument
    }
    return me->ops->set_send_id(me, frame_num_c,freq_us);
}

int ano_check_to_send(ano_base_t* me,uint8_t frame_num_c)
{
  CHECKIF(me == NULL || me->ops == NULL || me->ops->check_to_send == NULL) {
    return -EINVAL; //  Invalid argument
  }
    return me->ops->check_to_send(me, frame_num_c);
}

int ano_set_wts(ano_base_t* me,uint8_t frame_num_c)
{
    CHECKIF(me == NULL || me->ops == NULL || me->ops->set_wts == NULL) {
    return -EINVAL; //  Invalid argument
  }
    return me->ops->set_wts(me, frame_num_c);
}

// int ano_clear_wait_check(ano_base_t* me)
// {
//     CHECKIF(me == NULL || me->ops == NULL || me->ops->clear_wait_check == NULL) {
//     return -EINVAL; //  Invalid argument
//   }
//     return me->ops->clear_wait_check(me);
// }

int ano_get_check_back(ano_base_t *me,struct check_back_t* check_back_pst)
{
  CHECKIF(me == NULL || me->ops == NULL || me->ops->get_check_back == NULL) {
    return -EINVAL; //  Invalid argument
  }
  return me->ops->get_check_back(me,check_back_pst);
}

int ano_get_cmd(ano_base_t *me,struct command_t* cmd_pst)
{
    CHECKIF(me == NULL || me->ops == NULL || me->ops->get_cmd == NULL) {
    return -EINVAL; //  Invalid argument
  }
  return me->ops->get_cmd(me,cmd_pst);
}

int ano_get_par(ano_base_t *me,struct par_t* par_pst)
{
    CHECKIF(me == NULL || me->ops == NULL || me->ops->get_par == NULL) {
    return -EINVAL; //  Invalid argument
  }
  return me->ops->get_par(me,par_pst);
}

int ano_ck_back_check(ano_base_t*me)
{
    CHECKIF(me == NULL || me->ops == NULL || me->ops->ck_back_check == NULL) {
    return -EINVAL; //  Invalid argument
  }
  return me->ops->ck_back_check(me);
}


int ano_check_data(struct ano_base_t*me)
{
    CHECKIF(me == NULL || me->ops == NULL || me->ops->ano_check_data == NULL) {
    return -EINVAL; //  Invalid argument
  }

  return me->ops->ano_check_data(me);
}

int ano_clear_wait(ano_base_t *me)
{
  CHECKIF(me == NULL || me->ops == NULL || me->ops->clear_wait == NULL) {
    return -EINVAL; //  Invalid argument
  }
  return me->ops->clear_wait(me);
}



