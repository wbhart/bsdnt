#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

typedef struct bsdnt_cpuid_t
{
   int family;
   int model;
   int stepping;
} bsdnt_cpuid_t;

void cpuid(unsigned int index, unsigned int *v1, 
           unsigned int *v2, unsigned int *v3, unsigned int *v4)
{
    __asm__ __volatile__(
        "xchg %%ebx, %%edi;"
        "cpuid;"
        "xchg %%ebx, %%edi;"
        :"=a" (*v1), "=D" (*v2), "=c" (*v4), "=d" (*v3)
        :"0" (index)
    );
}

int main(void)
{
   unsigned int regs[4];
   unsigned int val;
   unsigned int dummy;
   bsdnt_cpuid_t fms;

   #define VendorID ((char *) regs)

   regs[3] = 0; /* terminate string */

   cpuid(0, &val, regs, regs + 1, regs + 2);

   cpuid(1, &val, &dummy, &dummy, &dummy);

   if (strcmp(VendorID, "AuthenticAMD") == 0)
   {
      fms.family = ((val>>8) & 15);
      fms.model = ((val>>4) & 15);
      if (fms.family == 15) 
      {
         fms.family += ((val>>20) & 255);
         fms.model += ((val>>12) & 240);
      }
      fms.stepping = val & 15;

      if (fms.family <= 6)
         printf("x86\n");
      else switch (fms.family)
      {
         case 15: printf("k8\n"); break;
         case 16: 
            if (fms.model >= 4)
               printf("k102\n");
            else
               printf("k10\n");
            break;
         case 17: printf("k8\n"); break;
         case 18: printf("k103\n"); break;
         case 20: printf("bobcat\n"); break;
         case 21: printf("bulldozer\n"); break;
         default: printf("unknown\n");
      }
   } else if (strcmp(VendorID, "GenuineIntel") == 0)
   {
      fms.family = ((val>>8) & 15) + ((val>>20) & 255);
      fms.model = ((val>>4) & 15) + ((val>>12) & 240);
      fms.stepping = val & 15;

      if (fms.family <= 5)
         printf("x86\n");
      else switch (fms.family)
      {
         case 6:
            if (fms.model <= 14)
               printf("x86\n");
            else
               switch (fms.model)
               {
               case 15: printf("core2\n"); break;
               case 22: printf("core2\n"); break;
               case 23: printf("core2\n"); break;
               case 26: printf("nehalem\n"); break;
               case 27: printf("nehalem\n"); break;
               case 28: printf("atom\n"); break;
               case 29: printf("core2\n"); break;
               case 30: printf("nehalem\n"); break;
               case 31: printf("nehalem\n"); break;
               case 37: printf("westmere\n"); break;
               case 38: printf("atom\n"); break;
               case 39: printf("atom\n"); break;
               case 42: printf("sandybridge\n"); break;
               case 44: printf("westmere\n"); break;
               case 45: printf("sandybridge\n"); break;
               case 46: printf("nehalem\n"); break;
               case 47: printf("westmere\n"); break;
               case 54: printf("atom\n"); break;
               case 58: printf("sandybridge\n"); break;
               default: printf("unknown\n");
               }
            break;
         case 15: printf("p4\n"); break;
         default: printf("unknown\n");
      }
   } else
      printf("unknown");

   return 0;
}



