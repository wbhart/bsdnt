#ifndef BSDNT_GENERIC_H
#define BSDNT_GENERIC_H

#include "nn.h"

typedef enum
{
   NN 
} type_t;

typedef enum
{
   ANY, NONZERO, ODD 
} flag_t;

typedef struct node_t
{
   type_t type;
   void * ptr;
   struct node_t * next;
} node_t;

void talker(const char * str);

void randoms_upto(word_t limit, flag_t flag, rand_t state, ...);

void randoms_of_len(len_t n, flag_t flag, rand_t state, ...);

void gc_cleanup(void);

#endif

