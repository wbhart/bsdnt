#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "generic.h"

node_t * garbage = NULL;

void talker(const char * str)
{
   fprintf(stderr, "Error: %s\n", str);
   abort();
}

void new_objs(type_t type, ...)
{
   va_list ap;
   obj_t ** ptr;

   va_start(ap, type);
   
   while ((ptr = va_arg(ap, obj_t **)) != NULL)
   {
      obj_t * obj = malloc(sizeof(obj_t));
      garbage = new_node(obj, garbage);
      obj->type = NIL;
      (*ptr) = obj;
   }

   va_end(ap);
}

void free_obj(obj_t * obj)
{
   if (obj->type == NN)
      free(obj->val.nn.ptr);
   
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
   default: talker("Unsupported object type in randoms.");
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
   case WORD:
      do {
         obj->val.nn.ptr = nn_init(n);
         nn_random(obj->val.nn.ptr, state, n);
      } while ((obj = va_arg(ap, obj_t *)) != NULL);
      break;
   default: talker("Unsupported object type in randoms.");
   }

   va_end(ap);
}