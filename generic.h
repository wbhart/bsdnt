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

typedef enum
{
   L, R, M 
} control_t;

typedef struct obj_t
{
   type_t type;
   union
   {
      struct
      {
         nn_t ptr;
         len_t len;
         int virt;
      } nn;
      word_t word;
   } val;
   control_t control;
} obj_t;

static inline
void talker(const char * str)
{
   fprintf(stderr, "Error: %s\n", str);
   abort();
}

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

void new_chain(obj_t * fob, ...);

int equal(obj_t * obj1, obj_t * obj2);

obj_t * add_m(obj_t * dest, obj_t * src1, obj_t * src2, obj_t * carry);

obj_t * sub_m(obj_t * dest, obj_t * src1, obj_t * src2, obj_t * carry);

obj_t * add(obj_t * dest, obj_t * src1, obj_t * src2, obj_t * carry);

obj_t * sub(obj_t * dest, obj_t * src1, obj_t * src2, obj_t * carry);

#define chain2_rl(fn1, fn2, ctl) \
   do { \
      ctl->control = R; \
      ctl = fn2; \
      ctl->control = L; \
      fn1; \
   } while (0)

#define chain2_lr(fn1, fn2, ctl) \
   do { \
      ctl->control = L; \
      ctl = fn1; \
      ctl->control = R; \
      fn2; \
   } while (0)

#define chain3_rl(fn1, fn2, fn3, ctl) \
   do { \
      ctl->control = R; \
      ctl = fn3; \
      ctl->control = M; \
      ctl = fn2; \
      ctl->control = R; \
      fn1; \
   } while (0)

#define chain3_lr(fn1, fn2, fn3, ctl) \
   do { \
      ctl->control = L; \
      ctl = fn1; \
      ctl->control = M; \
      ctl = fn2; \
      ctl->control = R; \
      fn3; \
   } while (0)

#endif

