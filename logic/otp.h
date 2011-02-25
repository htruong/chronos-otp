/*
 * otp.h
 * ---------------------------------- 
 * Google One Time Password Generator
 * Author: Huan Truong <hnt7438 at truman period edu>
 * Special thanks to:
 *    Dr Donald Bindner
 * Thanks to:
 *    Dr Robert Matthews 
 *       from Truman State University
 * 
 * SHA1_HMAC lib by Markus Gutschke, Google Inc.
 * SHA1 lib by 
 *    Steve Reid <sreid@sea-to-sky.net>
 *    James H. Brown <jbrown@burgoyne.com>
 *
 * WARNING: THIS IS A QUICK AND DIRTY HACK! 
 * JUST A PROOF OF CONCEPT, NOT MEANT TO BE TAKEN SERIOUSLY.
 * I WILL CLEAN THIS UP VERY SOON.
 * 
 */


#ifndef OTP_H_
#define OTP_H_

// *************************************************************************************************
// Extern section
extern void sx_otp(u8 line);
extern void display_otp(u8 line, u8 update);

// Base32 decode function
extern u8 base32_decode(const u8 *encoded, u8 *result, u8 bufSize);

// SHA-1 functions

#define     SHA1_BLOCKSIZE 64
#define SHA1_DIGEST_SIZE 20
#define SHA1_DIGEST_LENGTH 20

typedef struct {
    u32 state[5];
    u32 count[2];
    u8  buffer[SHA1_BLOCKSIZE];
} SHA1_CTX;

void SHA1_Init(SHA1_CTX* context);
void SHA1_Update(SHA1_CTX* context, const u8* data, const u32 len);
void SHA1_Final(SHA1_CTX* context, u8 digest[SHA1_DIGEST_SIZE]);


#endif /*otp_H_*/