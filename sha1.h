/*
 ---------------------------------------------------------------------------
 Copyright (c) 1998-2008, Brian Gladman, Worcester, UK. All rights reserved.

 LICENSE TERMS

 The redistribution and use of this software (with or without changes)
 is allowed without the payment of fees or royalties provided that:

  1. source code distributions include the above copyright notice, this
     list of conditions and the following disclaimer;

  2. binary distributions include the above copyright notice, this list
     of conditions and the following disclaimer in their documentation;

  3. the name of the copyright holder is not used to endorse products
     built using this software without specific written permission.

 DISCLAIMER

 This software is provided 'as is' with no explicit or implied warranties
 in respect of its properties, including, but not limited to, correctness
 and/or fitness for purpose.
 ---------------------------------------------------------------------------
 Issue Date: 20/12/2007
*/

#ifndef _SHA1_H
#define _SHA1_H

#ifdef __cplusplus
 extern "C" {
#endif

/* define for bit or byte oriented SHA   */
#if 1
#  define SHA1_BITS 0   /* byte oriented */
#else
#  define SHA1_BITS 1   /* bit oriented  */
#endif

#include <stdlib.h>
#include <stdint.h>

#define SHA1_BLOCK_SIZE  64
#define SHA1_DIGEST_SIZE 20

/* type to hold the SHA256 context  */

typedef struct
{   
	 uint32_t count[2];
    uint32_t hash[SHA1_DIGEST_SIZE >> 2];
    uint32_t wbuf[SHA1_BLOCK_SIZE >> 2];
} sha1_ctx;

/* 
   Note that these prototypes are the same for both bit and 
   byte oriented implementations. However the length fields 
   are in bytes or bits as appropriate for the version used 
   and bit sequences are input as arrays of bytes in which  
   bit sequences run from the most to the least significant 
   end of each byte. The value 'len' in sha1_hash for the   
   byte oriented version of SHA1 is limited to 2^29 bytes,  
   but multiple calls will handle longer data blocks.       
*/

void sha1_compile(sha1_ctx ctx[1]);

void sha1_begin(sha1_ctx ctx[1]);
void sha1_hash(const unsigned char data[], unsigned long len, sha1_ctx ctx[1]);
void sha1_end(unsigned char hval[], sha1_ctx ctx[1]);

/*
   Set hval[] to the 160 bit sha1 hash of data[] of length len bytes.
*/
void sha1(unsigned char hval[], const unsigned char data[], unsigned long len);

#ifdef __cplusplus
 }
#endif

#endif
