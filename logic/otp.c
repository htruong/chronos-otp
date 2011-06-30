/*
 * otp.c
 * ---------------------------------- 
 * Google One Time Password Generator
 * Author: Huan Truong <hnt7438 at truman period edu>
 * Special thanks to:
 *    Dr Donald Bindner
 * Thanks to:
 *    Dr Robert Matthews 
 *    Shannon Pamperl
 *       from Truman State University
 * 
 * SHA1_HMAC lib by Markus Gutschke, Google Inc.
 * SHA1 lib by 
 *    Steve Reid <sreid@sea-to-sky.net>
 *    James H. Brown <jbrown@burgoyne.com>
 *
 * WARNING: THIS IS STILL A QUICK AND DIRTY HACK! 
 * 
 */


#include <project.h>

#ifdef CONFIG_OTP

#include "otp.h"

#include "display.h"
#include "clock.h"
#include <string.h>
//////////////////////////////////////

struct otp_cache sOtp_cache;

//////////////////////////////////////

void SHA1_Transform(u32 state[5], const u8 buffer[64]);

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

#define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) \
    |(rol(block->l[i],8)&0x00FF00FF))
#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
    ^block->l[(i+2)&15]^block->l[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);


/* Hash a single 512-bit block. This is the core of the algorithm. */
void SHA1_Transform(u32 state[5], const u8 buffer[64])
{
    typedef union {
        u8 c[64];
        u32 l[16];
    } CHAR64LONG16;
    CHAR64LONG16* block;

    block = (CHAR64LONG16*)buffer;

    /* Copy context->state[] to working vars */
	
	u32 state2[5];
	state2[0] = state[0];
    state2[1] = state[1];
    state2[2] = state[2];
    state2[3] = state[3];
    state2[4] = state[4];
	
	
	// Have to roll the loop again, as we do not have enough space!
	u8 i;
	for (i =  0; i < 16; i++) { R0(state2[(80-i)%5],state2[(81-i)%5],state2[(82-i)%5],state2[(83-i)%5],state2[(84-i)%5], i); }
    for (i = 16; i < 20; i++) { R1(state2[(80-i)%5],state2[(81-i)%5],state2[(82-i)%5],state2[(83-i)%5],state2[(84-i)%5], i); }
    for (i = 20; i < 40; i++) { R2(state2[(80-i)%5],state2[(81-i)%5],state2[(82-i)%5],state2[(83-i)%5],state2[(84-i)%5], i); }
    for (i = 40; i < 60; i++) { R3(state2[(80-i)%5],state2[(81-i)%5],state2[(82-i)%5],state2[(83-i)%5],state2[(84-i)%5], i); }
    for (i = 60; i < 80; i++) { R4(state2[(80-i)%5],state2[(81-i)%5],state2[(82-i)%5],state2[(83-i)%5],state2[(84-i)%5], i); }    
	
	for (i =  0; i < 5; i++) {
	  state[i] += state2[i];
	}
	
	/* Wipe variables */
	state2[0] = state2[1] = state2[2] = state2[3] = state2[4] = 0; 
}


/* SHA1Init - Initialize new context */
void SHA1_Init(SHA1_CTX* context)
{
    /* SHA1 initialization constants */
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = context->count[1] = 0;
}


/* Run your data through this. */
void SHA1_Update(SHA1_CTX* context, const u8* data, const u32 len)
{
    size_t i, j;

    j = (context->count[0] >> 3) & 63;
    if ((context->count[0] += len << 3) < (len << 3)) context->count[1]++;
    context->count[1] += (len >> 29);
    if ((j + len) > 63) {
        memcpy(&context->buffer[j], data, (i = 64-j));
        SHA1_Transform(context->state, context->buffer);
        for ( ; i + 63 < len; i += 64) {
            SHA1_Transform(context->state, data + i);
        }
        j = 0;
    }
    else i = 0;
    memcpy(&context->buffer[j], &data[i], len - i);
}


/* Add padding and return the message digest. */
void SHA1_Final(SHA1_CTX* context, u8 digest[SHA1_DIGEST_SIZE])
{
    u32 i;
    u8  finalcount[8];

    for (i = 0; i < 8; i++) {
        finalcount[i] = (unsigned char)((context->count[(i >= 4 ? 0 : 1)]
         >> ((3-(i & 3)) * 8) ) & 255);  /* Endian independent */
    }
    SHA1_Update(context, (u8 *)"\200", 1);
    while ((context->count[0] & 504) != 448) {
        SHA1_Update(context, (u8 *)"\0", 1);
    }
    SHA1_Update(context, finalcount, 8);  /* Should cause a SHA1_Transform() */
    for (i = 0; i < SHA1_DIGEST_SIZE; i++) {
        digest[i] = (u8)
         ((context->state[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
    }

    /* Wipe variables */
    i = 0;
    memset(context->buffer, 0, 64);
    memset(context->state, 0, 20);
    memset(context->count, 0, 8);
    memset(finalcount, 0, 8);	/* SWR */
}


void hmac_sha1(const u8 *key, u16 keyLength,
               const u8 *data, u16 dataLength,
               u8 *result, u16 resultLength) {
  SHA1_CTX ctx;
  u8 hashed_key[SHA1_DIGEST_SIZE];
  if (keyLength > 64) {
    // The key can be no bigger than 64 bytes. If it is, we'll hash it down to
    // 20 bytes.
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, key, keyLength);
    SHA1_Final(&ctx, hashed_key);
    key = hashed_key;
    keyLength = SHA1_DIGEST_SIZE;
  }

  // The key for the inner digest is derived from our key, by padding the key
  // the full length of 64 bytes, and then XOR'ing each byte with 0x36.
  u8 tmp_key[64];
  u16 i;
  for (i = 0; i < keyLength; ++i) {
    tmp_key[i] = key[i] ^ 0x36;
  }
  memset(tmp_key + keyLength, 0x36, 64 - keyLength);

  // Compute inner digest
  SHA1_Init(&ctx);
  SHA1_Update(&ctx, tmp_key, 64);
  SHA1_Update(&ctx, data, dataLength);
  u8 sha[SHA1_DIGEST_SIZE];
  SHA1_Final(&ctx, sha);

  // The key for the outer digest is derived from our key, by padding the key
  // the full length of 64 bytes, and then XOR'ing each byte with 0x5C.
  for (i = 0; i < keyLength; ++i) {
    tmp_key[i] = key[i] ^ 0x5C;
  }
  memset(tmp_key + keyLength, 0x5C, 64 - keyLength);

  // Compute outer digest
  SHA1_Init(&ctx);
  SHA1_Update(&ctx, tmp_key, 64);
  SHA1_Update(&ctx, sha, SHA1_DIGEST_SIZE);
  SHA1_Final(&ctx, sha);

  // Copy result to output buffer and truncate or pad as necessary
  memset(result, 0, resultLength);
  if (resultLength > SHA1_DIGEST_SIZE) {
    resultLength = SHA1_DIGEST_SIZE;
  }
  memcpy(result, sha, resultLength);

  // Zero out all u16ernal data structures
  memset(hashed_key, 0, sizeof(hashed_key));
  memset(sha, 0, sizeof(sha));
  memset(tmp_key, 0, sizeof(tmp_key));
}

void sx_otp(u8 line)
{
  sOtp_cache.disp_scroll = (sOtp_cache.disp_scroll + 1) % 2;
  sOtp_cache.needs_screen_updated = 1;
  update_otp(line, DISPLAY_LINE_UPDATE_PARTIAL);
}

void recalculate_otp() 
{
  if (sOtp_cache.next_update > sTime.system_time) return;
  
  sOtp_cache.next_update = ((u16)(sTime.system_time / 30))*30 + 30;
  
  u8 *secret = sOtp_cache.secretkey;
  
  u32 tm = (sTime.system_time+SYSTEM_EPOCH_OFFSET)/30;
  u16 i = 0;
  //for (i = -1; i <= 1; ++i) {
    u8 challenge[8];
    u32 chlg = tm + i;
	u32 j;
	
    for (j = 8; j--; chlg >>= 8) {
      challenge[j] = chlg;
    }
    
    u8 hash[SHA1_DIGEST_SIZE];
    hmac_sha1(secret, OTP_KEY_LEN, challenge, 8, hash, SHA1_DIGEST_SIZE);

	u32 offset = hash[SHA1_DIGEST_SIZE - 1] & 0xF;
    u32 truncatedHash = 0;
    for (j = 0; j < 4; ++j) {
      truncatedHash <<= 8;
      truncatedHash  |= hash[offset + j];
    }
    
    memset(hash, 0, sizeof(hash));
    truncatedHash &= 0x7FFFFFFF;
    truncatedHash %= 1000000;
	sOtp_cache.disp_scroll = 0;
	sOtp_cache.last_hash = truncatedHash;
	sOtp_cache.needs_screen_updated = 1;
}

void update_otp(u8 line, u8 update)
{
	// The screen is kind of wacky and can only display 5 chars at once.
	// So we need to display 3 at a time [ -123] and [456- ]
	if (sOtp_cache.disp_scroll == 0) {
	  display_chars(switch_seg(line, LCD_SEG_L1_3_0, LCD_SEG_L2_4_0), (u8*)" -XXX", SEG_ON);
	  display_chars(switch_seg(line, LCD_SEG_L1_3_0, LCD_SEG_L2_2_0), itoa(sOtp_cache.last_hash / 1000, 3, 0), SEG_ON);
	} else {
	  display_chars(switch_seg(line, LCD_SEG_L1_3_0, LCD_SEG_L2_4_0), itoa(sOtp_cache.last_hash % 1000, 3, 0), SEG_ON);
	  display_chars(switch_seg(line, LCD_SEG_L1_3_0, LCD_SEG_L2_1_0), (u8*)"- ", SEG_ON);
	}
	
	sOtp_cache.needs_screen_updated = 0;
}



void display_otp(u8 line, u8 update)
{
  if (update == DISPLAY_LINE_UPDATE_PARTIAL) {
	if (sOtp_cache.needs_screen_updated) {
	  update_otp(line, update);
	}
  } else if (update == DISPLAY_LINE_UPDATE_FULL) {
	sOtp_cache.inactive = 0;
        update_otp(line, update);
  } else if (update == DISPLAY_LINE_CLEAR) {
	sOtp_cache.inactive = 1;
  }
}

#endif // CONFIG_OTP