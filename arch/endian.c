#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define LE_VALUE 0x0D0C0B0A
#define BE_VALUE 0x0A0B0C0D

int main(void)
{
   char arr[4] = { 0x0A, 0x0B, 0x0C, 0x0D };

   if (((uint32_t *) arr)[0] == LE_VALUE)
      printf("little\n");
   else if (((uint32_t *) arr)[0] == BE_VALUE)
      printf("big\n");
   else
   {
      printf("Error: machine is neither little nor big endian!\n");
      abort();
   }

   return 0;
}
