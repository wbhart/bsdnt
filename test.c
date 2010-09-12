#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "test.h"

node_t * garbage = NULL;

void talker(const char * str)
{
   fprintf(stderr, "Error: %s\n", str);
}

node_t * new_node(type_t type, void * ptr, node_t * next)
{
   node_t * node = malloc(sizeof(node_t));
   node->type = type;
   
   if (type == NN)
      node->ptr = ptr;
   
   node->next = next;

   return node;
}

void randoms_upto(word_t limit, flag_t flag, rand_t state, ...)
{
   va_list ap;
   word_t * w;

   va_start(ap, state);
   
   if (limit == 0)
      talker("Limit too low in randoms_upto");
      
   while ((w = va_arg(ap, word_t *)) != NULL) 
   {
      (*w) = randint(limit, state);

      switch (flag)
      {
      case ANY: break;

      case ODD: 
         if (limit == 1)
            talker("Limit too low in randoms_upto");
         (*w) |= 1;
         while ((*w) >= limit)
            (*w) = randint(limit, state) & 1; 
         break;

      case NONZERO: 
         if (limit == 1)
            talker("Limit too low in randoms_upto");
         while ((*w) == 0)
            (*w) = randint(limit, state); 
         break;

      default: talker("Unknown flag in randoms_upto.");
      }
   }

   va_end(ap);
}

void randoms_of_len(len_t n, flag_t flag, rand_t state, ...)
{
   va_list ap;
   nn_t * obj;

   if (flag != ANY)
   {
      talker("Type not implemented in randoms_of_len");
      abort();
   }

   va_start(ap, state);
   
   while ((obj = va_arg(ap, nn_t *)) != NULL) 
   {
      (*obj) = nn_init(n);
      nn_random(*obj, state, n);
      garbage = new_node(NN, (void *) (*obj), garbage);
   } 

   va_end(ap);
}

void free_obj(node_t * obj)
{
   if (obj->type == NN)
      free(obj->ptr);
}

void gc_cleanup(void)
{
   node_t * g = garbage;
   node_t * temp;

   while (g != NULL)
   {
      free_obj(g);

      temp = g;
      g = g->next;
      free(temp);
   }

   garbage = NULL;
}