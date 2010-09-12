#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "generic.h"
#include "nn.h"

node_t * garbage = NULL;

void new_objs(type_t type, ...)
{
   va_list ap;
   obj_t ** ptr;

   va_start(ap, type);
   
   while ((ptr = va_arg(ap, obj_t **)) != NULL)
   {
      obj_t * obj = malloc(sizeof(obj_t));
      garbage = new_node(obj, garbage);
      obj->type = type;
      (*ptr) = obj;
   }

   va_end(ap);
}

obj_t * new_ctl(control_t ctl)
{
   obj_t * obj = malloc(sizeof(obj_t));
   garbage = new_node(obj, garbage);
   obj->type = NIL;
   obj->control = ctl;

   return obj;
}

void free_obj(obj_t * obj)
{
   if (obj->type == NN)
   {
      if (!obj->val.nn.virt)
         free(obj->val.nn.ptr);
   }
   
   free(obj);
}

void gc_cleanup(void)
{
   node_t * g = garbage;
   node_t * temp;

   while (g != NULL)
   {
      free_obj(g->obj);

      temp = g;
      g = g->next;
      free(temp);
   }

   garbage = NULL;
}

void randoms(flag_t flag, rand_t state, ...)
{
   va_list ap;
   obj_t * obj;

   va_start(ap, state);
   obj = va_arg(ap, obj_t *);

   switch (obj->type)
   {
   case WORD:
      do {
         obj->val.word = randword(state);
         switch (flag)
         {
         case ANY: break;
         case ODD: obj->val.word |= (word_t) 1; break;
         case NONZERO: obj->val.word += (obj->val.word == (word_t) 0); break;
         default: talker("Unknown flag in randoms.");
         }
      } while ((obj = va_arg(ap, obj_t *)) != NULL);
      break;
   default: talker("Unsupported object type in randoms.");
   }

   va_end(ap);
}

void randoms_upto(obj_t * limit, flag_t flag, rand_t state, ...)
{
   va_list ap;
   obj_t * obj;

   va_start(ap, state);
   obj = va_arg(ap, obj_t *);

   switch (obj->type)
   {
   case WORD:
      do {
         obj->val.word = randint(limit->val.word, state);
         switch (flag)
         {
         case ANY: break;
         case ODD: 
            while ((obj->val.word & (word_t) 1) == (word_t) 0)
               obj->val.word = randint(limit->val.word, state); 
            break;
         case NONZERO: 
            while (obj->val.word == (word_t) 0)
               obj->val.word = randint(limit->val.word, state); 
            break;
         default: talker("Unknown flag in randoms.");
         }
      } while ((obj = va_arg(ap, obj_t *)) != NULL);
      break;
   default: talker("Unsupported object type in randoms_upto.");
   }

   va_end(ap);
}

void randoms_of_len(len_t n, flag_t flag, rand_t state, ...)
{
   va_list ap;
   obj_t * obj;

   va_start(ap, state);
   obj = va_arg(ap, obj_t *);

   switch (obj->type)
   {
   case NN:
      do {
         obj->val.nn.ptr = nn_init(n);
         obj->val.nn.len = n;
         obj->val.nn.virt = 0;
         nn_random(obj->val.nn.ptr, state, n);
      } while ((obj = va_arg(ap, obj_t *)) != NULL);
      break;
   default: talker("Unsupported object type in randoms_of_len.");
   }

   va_end(ap);
}

void new_chain(obj_t * fob, ...)
{
   va_list ap;
   obj_t * obj;
   len_t m;
   nn_t ptr = fob->val.nn.ptr;

   va_start(ap, fob);

   while ((obj = va_arg(ap, obj_t *)) != NULL)
   {
      obj->val.nn.ptr = ptr;
      obj->val.nn.virt = 1;
      
      m = va_arg(ap, len_t);
      obj->val.nn.len = m;

      ptr += m;
   } 

   va_end(ap);
}

int equal(obj_t * obj1, obj_t * obj2)
{
   if (obj1->type == WORD)
      return (obj1->val.word == obj2->val.word);
   else if (obj1->type == NN)
   {
      len_t len1, len2;

      len1 = nn_normalise(obj1->val.nn.ptr, obj1->val.nn.len);
      len2 = nn_normalise(obj2->val.nn.ptr, obj2->val.nn.len);

      return (nn_equal(obj1->val.nn.ptr, len1, 
          obj2->val.nn.ptr, len2));
   } else
      talker("Unsupported object type in equal.");
      
   return 0;
}

#define def_fn_1d_2s(name, l_fn, r_fn, m_fn, f_fn, op1) \
   obj_t * name(obj_t * dest, obj_t * src1, obj_t * src2, obj_t * carry) \
   { \
      obj_t * ret; \
      new_objs(NIL, &ret, NULL); \
      if (src1->type == WORD) \
      { \
         dest->val.word = src1->val.word op1 src2->val.word; \
      } else if (carry == NULL) \
      { \
         f_fn(dest->val.nn.ptr, src1->val.nn.ptr, src1->val.nn.len, \
            src2->val.nn.ptr, src2->val.nn.len); \
      } else if (src1->type == NN) \
      { \
         switch (carry->control) \
         { \
         case R:  ret->type = WORD; \
            ret->val.word = r_fn(dest->val.nn.ptr, src1->val.nn.ptr, \
               src1->val.nn.len, src2->val.nn.ptr, src2->val.nn.len); \
               break; \
         case L: l_fn(dest->val.nn.ptr, src1->val.nn.ptr, src1->val.nn.len, \
            src2->val.nn.ptr, src2->val.nn.len, carry->val.word); \
               break; \
         case M: ret->type = WORD; \
            ret->val.word = m_fn(dest->val.nn.ptr, src1->val.nn.ptr, \
               src1->val.nn.len, src2->val.nn.ptr, src2->val.nn.len, \
                   carry->val.word); \
               break; \
         } \
      } \
      return ret; \
   }
 
def_fn_1d_2s(add, nn_add_c, _nn_add, _nn_add_c, nn_add, +)
      
def_fn_1d_2s(sub, nn_sub_c, _nn_sub, _nn_sub_c, nn_sub, -)

#define def_fn_1d_2s_m(name, l_fn, r_fn, m_fn, f_fn, op1) \
   obj_t * name(obj_t * dest, obj_t * src1, obj_t * src2, obj_t * carry) \
   { \
      obj_t * ret; \
      new_objs(NIL, &ret, NULL); \
      if (carry == NULL) \
         f_fn(dest->val.nn.ptr, src1->val.nn.ptr, \
            src2->val.nn.ptr, src2->val.nn.len); \
      else if (src1->type == NN) \
      { \
         switch (carry->control) \
         { \
         case R:  ret->type = WORD; \
            ret->val.word = r_fn(dest->val.nn.ptr, src1->val.nn.ptr, \
               src2->val.nn.ptr, src2->val.nn.len); \
               break; \
         case L: l_fn(dest->val.nn.ptr, src1->val.nn.ptr, \
            src2->val.nn.ptr, src2->val.nn.len, carry->val.word); \
               break; \
         case M: ret->type = WORD; \
            ret->val.word = m_fn(dest->val.nn.ptr, src1->val.nn.ptr, \
               src2->val.nn.ptr, src2->val.nn.len, carry->val.word); \
               break; \
         } \
      } \
      return ret; \
   }
  
def_fn_1d_2s_m(add_m, nn_add_mc, _nn_add_m, _nn_add_mc, nn_add_m, +)
      
def_fn_1d_2s_m(sub_m, nn_sub_mc, _nn_sub_m, _nn_sub_mc, nn_sub_m, -)

#define def_fn_1d_1s(name, l_fn, r_fn, m_fn, f_fn, op1) \
   obj_t * name(obj_t * dest, obj_t * src1, obj_t * carry) \
   { \
      obj_t * ret; \
      new_objs(NIL, &ret, NULL); \
      if (src1->type == WORD) \
      { \
         dest->val.word = op1 src1->val.word; \
      } else if (carry == NULL) \
      { \
         f_fn(dest->val.nn.ptr, src1->val.nn.ptr, src1->val.nn.len); \
      } else if (src1->type == NN) \
      { \
         switch (carry->control) \
         { \
         case R:  ret->type = WORD; \
            ret->val.word = r_fn(dest->val.nn.ptr, src1->val.nn.ptr, \
               src1->val.nn.len); \
               break; \
         case L: l_fn(dest->val.nn.ptr, src1->val.nn.ptr, src1->val.nn.len, \
            carry->val.word); \
               break; \
         case M: ret->type = WORD; \
            ret->val.word = m_fn(dest->val.nn.ptr, src1->val.nn.ptr, \
               src1->val.nn.len, carry->val.word); \
               break; \
         } \
      } \
      return ret; \
   }

def_fn_1d_1s(neg, nn_neg_c, _nn_neg, _nn_neg_c, nn_neg, -)

#define def_fn_1d_1s_c(name, l_fn, r_fn, m_fn, f_fn, op1) \
   obj_t * name(obj_t * dest, obj_t * src1, word_t c, obj_t * carry) \
   { \
      obj_t * ret; \
      new_objs(NIL, &ret, NULL); \
      if (src1->type == WORD) \
      { \
         dest->val.word = src1->val.word op1 c; \
      } else if (carry == NULL) \
      { \
         f_fn(dest->val.nn.ptr, src1->val.nn.ptr, \
             MIN(src1->val.nn.len, dest->val.nn.len), c); \
      } else if (src1->type == NN) \
      { \
         switch (carry->control) \
         { \
         case R:  ret->type = WORD; \
            ret->val.word = r_fn(dest->val.nn.ptr, src1->val.nn.ptr, \
               MIN(src1->val.nn.len, dest->val.nn.len), c); \
               break; \
         case L: l_fn(dest->val.nn.ptr, src1->val.nn.ptr, \
         MIN(src1->val.nn.len, dest->val.nn.len), c, carry->val.word); \
               break; \
         case M: ret->type = WORD; \
            ret->val.word = m_fn(dest->val.nn.ptr, src1->val.nn.ptr, \
               MIN(src1->val.nn.len, dest->val.nn.len), c, carry->val.word); \
               break; \
         } \
      } \
      return ret; \
   }

def_fn_1d_1s_c(shl, nn_shl_c, _nn_shl, _nn_shl_c, nn_shl, <<)

void dud_c(nn_t r, nn_t a, len_t n, word_t c, word_t ci)
{
   talker("No such function");
}

def_fn_1d_1s_c(shr, dud_c, _nn_shr, _nn_shr_c, nn_shr, >>)

