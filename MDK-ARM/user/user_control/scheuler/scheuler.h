#ifndef __SCHEULER_H
#define __SCHEULER_H
#include "stdint.h"
#include  "main.h"

struct ano_event_t
{
    uint16_t ano_base;
    uint16_t ano_id;   
};

typedef uint16_t event_id_t;

struct event_t {
  event_id_t id;
  uint32_t param;
};



#endif