#include <stdio.h>
#include "helper.h"
#include "helper_arch.h"

/**********************************************************************
 
    Printing functions

**********************************************************************/

#ifndef HAVE_ARCH_printx_word

void printx_word(word_t a)
{
   printf("%lx", a);
}

#endif