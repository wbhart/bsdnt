#include <stdio.h>
#include <stdlib.h>

typedef struct cpuid_t
{
   int family;
   int model;
   int stepping;
} cpuid_t;

int main(void)
{
   char VendorID[13];
   int val;
   cpuid_t fms;

   VendorID[12] = '\0';

#if ULONG_MAX == 4294967295U /* 32 bits */
   __asm( 
          "push %%eax;"
          "mov $0, %%eax;"
          "cpuid;"
          "mov %%ebx, %0;"
          "mov %%edx, %1;"
          "mov %%ecx, %2;"
          "pop %%eax;"
          : "=m"(*(int*)(VendorID)), "=m"(*(int*)(VendorID+4)), "=m"(*(int*)(VendorID+8))
          :
          : "%ebx", "%edx", "%ecx"
   );
#else /* 64 bits */
   __asm( 
          "mov $0, %%eax;"
          "cpuid;"
          "mov %%ebx, %0;"
          "mov %%edx, %1;"
          "mov %%ecx, %2;"
          : "=m"(*(int*)(VendorID)), "=m"(*(int*)(VendorID+4)), "=m"(*(int*)(VendorID+8))
          :
          : "%eax", "%ebx", "%edx", "%ecx"
   );
#endif

   __asm( "mov $1, %%eax;"
          "cpuid;"
          "mov %%eax, %0;"
          : "=r"(val)
          :
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



