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

/*
   Print out an error message to stderr.
*/
static inline
void talker(const char * str)
{
   fprintf(stderr, "Error: %s\n", str);
}

/*
   Given a type and a null terminated list of *pointers* to (obj_t *)'s, 
   this function generates new objects of the given type and (invisibly) 
   adds references to them to the garbage stack. They can all be 
   automatically cleaned up by explicitly calling the garbage collector.

   obj_t * a, * b, * c;
   new_objs(NN, &a, &b, &c, NULL);
   // use a, b, c
   gc_cleanup();
*/
void new_objs(type_t type, ...);

typedef struct node_t
{
   obj_t * obj;
   struct node_t * next;
} node_t;

/*
   Given an object obj and a pointer to a node, this returns a node
   containing the given object which is linked to the given node.
   This is used to create a linked list (stack) of objects.
*/
static inline
node_t * new_node(obj_t * obj, node_t * next)
{
   node_t * node = malloc(sizeof(node_t));
   node->obj = obj;
   node->next = next;

   return node;
}

/*
   This function cleans up all objects which have been initialised
   by new_objs and the entire garbage stack. There is no function to
   explictly free an individual object. The garbage collector simply
   cleans up all objects that have been allocated. Beware that it 
   has no way of telling whether an object is still in use (this is
   C -- it would be difficult to do it any other way, because of
   pointers).
*/
void gc_cleanup(void);

/*
   Returns true is the object is of type NIL. Otherwise returns 0.
*/
static inline
int is_nil(obj_t * obj)
{
   return obj->type == NIL;
}

/*
   Returns true is the object is of type WORD. Otherwise returns 0.
*/
static inline
int is_word(obj_t * obj)
{
   return obj->type == WORD;
}

/*
   Returns true is the object is of type NN. Otherwise returns 0.
*/
static inline
int is_NN(obj_t * obj)
{
   return obj->type == NN;
}

/*
   Given a NULL terminated list of objects, this sets them to random 
   values. The flag specifies what type of values are allowed, e.g. 
   ODD, NONZERO, etc.

   randoms(ODD, state, a, b, c, NULL);
*/
void randoms(flag_t flag, rand_t state, ...);

/*
   Given an object limit, specifying a limit, this function sets a list
   of objects to random values constrained by that limit. The flag
   specifies what type of values are allowed, e.g. ODD, NONZERO, etc.

   randoms_upto(p, ODD, state, a, b, c, NULL);
*/
void randoms_upto(obj_t * limit, flag_t flag, rand_t state, ...);

/*
   Given a length n, this function sets a list of objects to random 
   values of the given length, e.g. multiple precision integers with
   the given number of limbs. 

   n = 100;
   randoms_of_len(n, ODD, state, a, b, c, NULL);
*/
void randoms_of_len(len_t n, flag_t flag, rand_t state, ...);

/*
   This function takes an already initialised object fob and a NULL 
   terminated list of object, length pairs. The object fob is broken 
   up into lengths corresponding to the lengths specified in the list 
   and the corresponding objects in the list are then made to point to 
   these segments. No data is copied. The objects simply act as windows 
   on the contents of fob. The garbage collector is designed to know
   not to try and free the space the attached objects point to, only the 
   space allocated inside fob.

   new_chain(fob, a, len1, b, len2, NULL);
*/
void new_chain(obj_t * fob, ...);

/*
   Returns 1 if the objects are equal (in the arithmetic sense, i.e.
   equal not identical).
*/
int equal(obj_t * obj1, obj_t * obj2);

/*
   Set dest to src1 + src2 assuming src1 and src2 are objects of the
   same length. The carry object can be NULL in which case a NIL object
   is returned and no carry-in need be supplied. Otherwise the carry
   object may have its control field set to L, R or M depending on
   whether the function is expected to accept a carry-in, return a 
   carry-out, or do both, respectively. Any carry-in value is to be
   passed in as the value of carry and the returned object will contain
   any carry-out.
*/
obj_t * add_m(obj_t * dest, obj_t * src1, obj_t * src2, obj_t * carry);

/*
   As per add_m but sets dest to src1 - src2.
*/
obj_t * sub_m(obj_t * dest, obj_t * src1, obj_t * src2, obj_t * carry);

/*
   As per add_m but src2 may be of a differnnt length to src1 (up to
   and including the length of src1).
*/
obj_t * add(obj_t * dest, obj_t * src1, obj_t * src2, obj_t * carry);

/*
   As per sub but the operation is subtraction.
*/
obj_t * sub(obj_t * dest, obj_t * src1, obj_t * src2, obj_t * carry);

/*
   Chains two function calls together, sending the carry-out of the 
   call fn2 to the carry-in of fn1. The object ctl must be supplied 
   to both the macro and as the final parameter to both the fn1 and
   fn2 calls.

   chain2_rl(add_m(r2, a2, b2, ctl), add_m(r1, a1, b1, ctl), ctl);
*/
#define chain2_rl(fn1, fn2, ctl) \
   do { \
      ctl->control = R; \
      ctl = fn2; \
      ctl->control = L; \
      fn1; \
   } while (0)

/*
   Automatically chains two lots of fn(ri, ai, bi, ctl) for i = 1..2 
   together from right to left, where the chains are divided into 
   lengths m, n (from left to right).
*/
#define chain2_rl_1d_2s(fn, r, a, b, m, n) \
   do { \
      obj_t * __r1, * __r2, * __a1, * __a2, * __b1, * __b2, * __ctl; \
      new_objs(a->type, &__r1, &__r2, &__a1, &__a2, &__b1, &__b2, NULL); \
      new_objs(NIL, &__ctl, NULL); \
      new_chain(a, __a1, m, __a2, n, NULL); \
      new_chain(b, __b1, m, __b2, n, NULL); \
      new_chain(r, __r1, m, __r2, n, NULL); \
      chain2_rl(fn(__r2, __a2, __b2, __ctl), \
                fn(__r1, __a1, __b1, __ctl), __ctl); \
   } while (0)

/*
   Chains two function calls together, sending the carry-out of the 
   call fn1 to the carry-in of fn2. The object ctl must be supplied 
   to both the macro and as the final parameter to both the fn1 and
   fn2 calls.
*/
#define chain2_lr(fn1, fn2, ctl) \
   do { \
      ctl->control = L; \
      ctl = fn1; \
      ctl->control = R; \
      fn2; \
   } while (0)

/*
   Automatically chains two lots of fn(ri, ai, bi, ctl) for i = 1..2 
   together from left to right, where the chains are divided into 
   lengths m, n (from left to right).
*/
#define chain2_lr_1d_2s(fn, r, a, b, m, n) \
   do { \
      obj_t * __r1, * __r2, * __a1, * __a2, * __b1, * __b2, * __ctl; \
      new_objs(a->type, &__r1, &__r2, &__a1, &__a2, &__b1, &__b2, NULL); \
      new_objs(NIL, &__ctl, NULL); \
      new_chain(a, __a1, m, __a2, n, NULL); \
      new_chain(b, __b1, m, __b2, n, NULL); \
      new_chain(r, __r1, m, __r2, n, NULL); \
      chain2_lr(fn(__r2, __a2, __b2, __ctl), \
                fn(__r1, __a1, __b1, __ctl), __ctl); \
   } while (0)

/*
   Chains three function calls together, sending the carry-out of the 
   call fn3 to the carry-in of fn1 and the carry-out of fn2 to the
   carry-in of fn1. The object ctl must be supplied to both the macro 
   and as the final parameter to the fn1, fn2 and fn3 calls.
*/
#define chain3_rl(fn1, fn2, fn3, ctl) \
   do { \
      ctl->control = R; \
      ctl = fn3; \
      ctl->control = M; \
      ctl = fn2; \
      ctl->control = L; \
      fn1; \
   } while (0)

/*
   Automatically chains three lots of fn(ri, ai, bi, ctl) for i = 1..3 
   together from right to left, where the chains are divided into 
   lengths m, n, p (from left to right).
*/
#define chain3_rl_1d_2s(fn, r, a, b, m, n, p) \
   do { \
      obj_t * __r1, * __r2, * __r3, * __a1, * __a2,  * __a3, \
                 * __b1, * __b2, * __b3, * __ctl; \
      new_objs(a->type, &__r1, &__r2, &__r3, \
         &__a1, &__a2, &__a3, &__b1, &__b2, &__b3, NULL); \
      new_objs(NIL, &__ctl, NULL); \
      new_chain(a, __a1, m, __a2, n, __a3, p, NULL); \
      new_chain(b, __b1, m, __b2, n, __b3, p, NULL); \
      new_chain(r, __r1, m, __r2, n, __r3, p, NULL); \
      chain3_rl(fn(__r3, __a3, __b3, __ctl), fn(__r2, __a2, __b2, __ctl), \
                fn(__r1, __a1, __b1, __ctl), __ctl); \
   } while (0)

/*
   Chains three function calls together, sending the carry-out of the 
   call fn1 to the carry-in of fn2 and the carry-out of fn2 to the
   carry-in of fn3. The object ctl must be supplied to both the macro 
   and as the final parameter to the fn1, fn2 and fn3 calls.
*/
#define chain3_lr(fn1, fn2, fn3, ctl) \
   do { \
      ctl->control = L; \
      ctl = fn1; \
      ctl->control = M; \
      ctl = fn2; \
      ctl->control = R; \
      fn3; \
   } while (0)

/*
   Automatically chains three lots of fn(ri, ai, bi, ctl) for i = 1..3 
   together from left to right, where the chains are divided into 
   lengths m, n, p (from left to right).
*/
#define chain3_lr_1d_2s(fn, r, a, b, m, n, p) \
   do { \
      obj_t * __r1, * __r2, * __r3, * __a1, * __a2,  * __a3, \
                 * __b1, * __b2, * __b3, * __ctl; \
      new_objs(a->type, &__r1, &__r2, &__r3, \
         &__a1, &__a2, &__a3, &__b1, &__b2, &__b3, NULL); \
      new_objs(NIL, &__ctl, NULL); \
      new_chain(a, __a1, m, __a2, n, __a3, p, NULL); \
      new_chain(b, __b1, m, __b2, n, __b3, p, NULL); \
      new_chain(r, __r1, m, __r2, n, __r3, p, NULL); \
      chain3_rl(fn(__r3, __a3, __b3, __ctl), fn(__r2, __a2, __b2, __ctl), \
                fn(__r1, __a1, __b1, __ctl), __ctl); \
   } while (0)

#endif

