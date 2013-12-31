/* 
  Copyright (C) 2010, William Hart
  Copyright (C) 2010, Brian Gladman

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice, 
     this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
	 documentation and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "helper.h"
#include "helper_arch.h"
#include "nn.h"

/**********************************************************************
 
    Printing functions

**********************************************************************/

void talker(const char * str)
{
   fprintf(stderr, "Error: %s\n", str);
}

/* return number of arguments called for by a specific format specifier */
int parse_fmt(int * floating, const char * fmt)
{
   int args = 1;
   
   fmt++; /* skip % */
   
   if (fmt[0] == '%')
	  return 0; /* special case, print % */

   if (fmt[0] == ' ' || fmt[0] == '+' || fmt[0] == '-')
	  fmt++; /* skip flag */

   if (fmt[0] == '*') 
   {
	   args++;
	   fmt++; /* skip * */
   } else
	  while (isdigit((unsigned char) fmt[0]))
		 fmt++; /* skip width */

   if (fmt[0] == '.')
   {
      fmt++; /* skip . */
	  if (fmt[0] == '*')
	  {
		 args++;
	     fmt++; /* skip * */
      } else
         while (isdigit((unsigned char) fmt[0]))
		    fmt++; /* skip precision */
   } 

   if (fmt[0] == 'h' || fmt[0] == 'l' || fmt[0] == 'L')
	  fmt++; /* skip length */

   if (fmt[0] == 'e' || fmt[0] == 'E' || fmt[0] == 'f' || fmt[0] == 'g' || fmt[0] == 'G')
	  (*floating) = 1;
   else 
	  (*floating) = 0;

   return args;
}

void bsdnt_printf(const char * str, ...)
{
   va_list ap;
   size_t len = strlen(str);
   char * str2 = (char *) malloc(len + 1);
   int w1 = 0, w2 = 0;
   void * w3;
   double d;
   word_t w;
   bits_t b;
   len_t m;
   int args, floating;

   /* deal with first substring */
   size_t n = strcspn(str, "%");
   strncpy(str2, str, n);
   str2[n] = '\0';
   printf("%s", str2);
   len -= n;
   str += n;

   va_start(ap, str);

   while (len) /* deal with fmt spec prefixed strings */
   {
	  n = strcspn(str + 2, "%") + 2; /* be sure to skip a %% */
	  strncpy(str2, str, n);
     str2[n] = '\0';
	
     switch (str[1])
	  {
	  case 'w':
		  w = (word_t) va_arg(ap, word_t);
		  if (str[2] == 'x')
		  {
			 printf(WORD_FMT "x", w);
			 printf("%s", str2 + 3);
		  } else
		  {
			 printf(WORD_FMT "d", w);
			 printf("%s", str2 + 2);
		  }
		  break;
	  case 'b':
		  b = (bits_t) va_arg(ap, bits_t);
		  printf(BITS_FMT, b);
        printf("%s", str2 + 2);
		  break;
	  case 'm':
		  m = (len_t) va_arg(ap, len_t);
		  printf(LEN_FMT, m);
        printf("%s", str2 + 2);
		  break;
	  default: /* pass to printf */
		  args = parse_fmt(&floating, str2);
		  if (args) 
		  {
			 if (args == 3)
	            w1 = va_arg(ap, int);
             if (args >= 2)
		        w2 = va_arg(ap, int);
             if (floating)
			 {
				 d = va_arg(ap, double);
				 if (args == 2)
			        printf(str2, w2, d);
				 else if (args == 3)
			        printf(str2, w1, w2, d);
				 else
					printf(str2, d);
			 } else
			 {
				 w3 = va_arg(ap, void *);
			    if (args == 2)
			        printf(str2, w2, w3);
				 else if (args == 3)
			        printf(str2, w1, w2, w3);
				 else
					printf(str2, w3);
			 }
	      } else printf("%s", str2); /* zero args */
	  }

	  len -= n;
      str += n;
   }

   va_end(ap);
   free(str2);
}

#ifndef HAVE_NATIVE_precompute_inverse2

preinv2_t precompute_inverse2(word_t d1, word_t d2)
{
   word_t q, r[2], p[2], ci;
   dword_t t;

   if (d2 + 1 == 0 && d1 + 1 == 0)
      return 0;

   if (d1 + 1 == 0)
      q = ~d1, r[1] = ~d2;
   else
   {
      t = ((((dword_t) ~d1) << WORD_BITS) + (dword_t) ~d2);
      q = (word_t) (t / (dword_t) (d1 + 1));
      r[1] = (word_t) (t % (dword_t) (d1 + 1));
   }

   if (d2 + 1 == 0)
      return q;
   
   r[0] = 0;

   t = (dword_t) q * (dword_t) (~d2);
   p[0] = (word_t) t;
   p[1] = (word_t) (t >> WORD_BITS);
   ci = nn_add_m(r, r, p, 2);
 
   p[0] = d2 + 1, p[1] = d1 + (d2 + 1 == 0);
   while (ci || nn_cmp_m(r, p, 2) >= 0)
   {
      q++;
      ci -= nn_sub_m(r, r, p, 2);
   }
   
   return q;
}

#endif
