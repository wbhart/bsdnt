#ifndef BSDNT_GENERIC_H
#define BSDNT_GENERIC_H

#include "nn.h"

typedef enum
{
   NIL, WORD, NN 
} type_t;

typedef enum
{
   ANY, NONZERO, ODD 
} flag_t;

typedef struct obj_t
{
   type_t type;
   union
   {
      struct
      {
         nn_t ptr;
         len_t len;
      } nn;
      word_t word;
   } val;
} obj_t;

void new_objs(type_t type, ...);

void free_obj(obj_t * obj);

typedef struct node_t
{
   obj_t * obj;
   struct node_t * next;
} node_t;

static inline
node_t * new_node(obj_t * obj, node_t * next)
{
   node_t * node = malloc(sizeof(node_t));
   node->obj = obj;
   node->next = next;

   return node;
}

void gc_cleanup(void);

static inline
int is_nil(obj_t * obj)
{
   return obj->type == NIL;
}

static inline
int is_word(obj_t * obj)
{
   return obj->type == WORD;
}

static inline
int is_NN(obj_t * obj)
{
   return obj->type == NN;
}

void randoms(flag_t flag, rand_t state, ...);

void randoms_upto(obj_t * limit, flag_t flag, rand_t state, ...);

void randoms_of_len(len_t n, flag_t flag, rand_t state, ...);

#endif

