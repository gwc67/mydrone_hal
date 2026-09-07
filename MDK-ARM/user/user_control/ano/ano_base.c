#include "ano_base.h"
#include "stdio.h"
#include "mesc.h"
int ano_send_cmd(struct ano_base_t*me, struct cmd_t* cmd_pst)
{
    CHECKIF(me == NULL || me->ops == NULL || me->ops->send_cmd == NULL ) {
        return  -EINVAL;                    //  Invalid argument
    }
    return me->ops->send_cmd(me,cmd_pst);
}

int ano_set_send2check(struct ano_base_t*me, struct ck_t* ck_pst)
{
    CHECKIF(me == NULL || me->ops == NULL ||  me->ops->set_send2check== NULL || ck_pst == NULL) {
        return  -EINVAL;                    //  Invalid argument
    }
    return me->ops->set_send2check(me, ck_pst);
}

int ano_set_par(ano_base_t*me, struct par_t* par_pst)
{
    CHECKIF(me == NULL || me->ops == NULL || me->ops->set_par == NULL ) {
        return  -EINVAL;                    //  Invalid argument
    }
    return me->ops->set_par(me, par_pst);
}

int ano_set_send_id(struct ano_base_t* me,uint8_t frame,enum event_id_e event_id_e,uint8_t prio)
{
    CHECKIF(me == NULL || me->ops == NULL || me->ops->set_send_id == NULL ) {
        return  -EINVAL;                    //  Invalid argument
    }
    return me->ops->set_send_id(me, frame,event_id_e,prio);
}

int ano_send_data(ano_base_t* me, uint8_t frame)
{
  CHECKIF(me == NULL || me->ops == NULL || me->ops->send_data == NULL){
    return -EINVAL;
  }
  return me->ops->send_data(me,frame);
}

int ano_data_setWts(struct ano_base_t* me,uint8_t frame)
{
    CHECKIF(me == NULL || me->ops == NULL || me->ops->data_SetWts == NULL) {
    return -EINVAL; //  Invalid argument
  }
    return me->ops->data_SetWts(me, frame);
}

int ano_get_send2check(struct ano_base_t* me , struct ck_t* ck_pst)
{
  CHECKIF(me == NULL || me->ops == NULL || me->ops->get_send2check == NULL) {
    return -EINVAL; //  Invalid argument
  }
  return me->ops->get_send2check(me,ck_pst);
}

int ano_get_cmd(ano_base_t *me,struct cmd_t* cmd_pst)
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

// int ano_ck_back_check(ano_base_t*me)
// {
//     CHECKIF(me == NULL || me->ops == NULL || me->ops->ck_back_check == NULL) {
//     return -EINVAL; //  Invalid argument
//   }
//   return me->ops->ck_back_check(me);
// }


// int ano_check_data(struct ano_base_t*me)
// {
//     CHECKIF(me == NULL || me->ops == NULL || me->ops->ano_check_data == NULL) {
//     return -EINVAL; //  Invalid argument
//   }

//   return me->ops->ano_check_data(me);
// }

// int ano_clear_wait(ano_base_t *me)
// {
//   CHECKIF(me == NULL || me->ops == NULL || me->ops->clear_wait == NULL) {
//     return -EINVAL; //  Invalid argument
//   }
//   return me->ops->clear_wait(me);
// }



