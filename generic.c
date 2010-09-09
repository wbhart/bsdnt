#include <stdarg.h>
#include "generic.h"

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
      obj->type = NIL;
      (*ptr) = obj;
   }
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
