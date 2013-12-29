#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct cpuid_t
{
   int family;
   int model;
   int stepping;
} cpuid_t;

int main(void)
{
   unsigned int regs[5];
   unsigned int val;
   unsigned int index = 0;
   cpuid_t fms;

   regs[4] = 0; /* terminate string */

#define VendorID ((char *) (regs + 1))

   __asm__ __volatile__( 
#if ULONG_MAX == 4294967295U /* 32 bit */
          "pushl %%ebx;"
#else /* 64 bit */
          "pushq %%rbx;"
#endif
          "cpuid;"
          "mov %%ebx, %[ebx];"
#if ULONG_MAX == 4294967295U /* 32 bit unsigned long */
          "popl %%ebx;"
#else /* 64 bit unsigned long */
          "popq %%rbx;"
#endif
          : "=a"(regs[0]), [ebx] "=r"(regs[1]), "=c"(regs[3]), "=d"(regs[2])
          : "a"(index)
   );

   index = 1; 
   __asm__ __volatile__( "cpuid;"
          "mov %%eax, %0;"
          : "=r"(val)
          : "0"(index)
          : "%eax"
   );

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
         case 16: printf("k10\n"); break;
         case 17: printf("k8\n"); break;
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
               case 37: printf("nehalem\n"); break;
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



