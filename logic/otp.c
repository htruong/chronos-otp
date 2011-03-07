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
 * WARNING: THIS IS A QUICK AND DIRTY HACK! 
 * JUST A PROOF OF CONCEPT, NOT MEANT TO BE TAKEN SERIOUSLY.
 * I WILL CLEAN THIS UP VERY SOON.
 * 
 */


#include <project.h>

#ifdef CONFIG_OTP

#include "otp.h"

#include "display.h"
#include "clock.h"
#include <string.h>
//////////////////////////////////////



//////////////////////////////////////
// HUEG COPY PASTA


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
	//printf ("state2 before %d %d %d %d %d\n", state2[0], state2[1], state2[2], state2[3], state2[4]);
	u8 i;
	for (i =  0; i < 16; i++) { R0(state2[(80-i)%5],state2[(81-i)%5],state2[(82-i)%5],state2[(83-i)%5],state2[(84-i)%5], i); }
    for (i = 16; i < 20; i++) { R1(state2[(80-i)%5],state2[(81-i)%5],state2[(82-i)%5],state2[(83-i)%5],state2[(84-i)%5], i); }
    for (i = 20; i < 40; i++) { R2(state2[(80-i)%5],state2[(81-i)%5],state2[(82-i)%5],state2[(83-i)%5],state2[(84-i)%5], i); }
    for (i = 40; i < 60; i++) { R3(state2[(80-i)%5],state2[(81-i)%5],state2[(82-i)%5],state2[(83-i)%5],state2[(84-i)%5], i); }
    for (i = 60; i < 80; i++) { R4(state2[(80-i)%5],state2[(81-i)%5],state2[(82-i)%5],state2[(83-i)%5],state2[(84-i)%5], i); }    
	//printf ("state2 after  %d %d %d %d %d\n", state2[0], state2[1], state2[2], state2[3], state2[4]);
	
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
  u8 hashed_key[SHA1_DIGEST_LENGTH];
  if (keyLength > 64) {
    // The key can be no bigger than 64 bytes. If it is, we'll hash it down to
    // 20 bytes.
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, key, keyLength);
    SHA1_Final(&ctx, hashed_key);
    key = hashed_key;
    keyLength = SHA1_DIGEST_LENGTH;
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
  u8 sha[SHA1_DIGEST_LENGTH];
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
  SHA1_Update(&ctx, sha, SHA1_DIGEST_LENGTH);
  SHA1_Final(&ctx, sha);

  // Copy result to output buffer and truncate or pad as necessary
  memset(result, 0, resultLength);
  if (resultLength > SHA1_DIGEST_LENGTH) {
    resultLength = SHA1_DIGEST_LENGTH;
  }
  memcpy(result, sha, resultLength);

  // Zero out all u16ernal data structures
  memset(hashed_key, 0, sizeof(hashed_key));
  memset(sha, 0, sizeof(sha));
  memset(tmp_key, 0, sizeof(tmp_key));
}















u8 base32_decode(const u8 *encoded, u8 *result, u8 bufSize) 
{
  u16 buffer = 0;
  u8 bitsLeft = 0;
  u8 count = 0;
  u8 *ptr;
  for (ptr = encoded; count < bufSize && *ptr; ++ptr) {
    u8 ch = *ptr;
    if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' || ch == '-') {
      continue;
    }
    buffer <<= 5;

    // Look up one base32 digit
    if (ch >= 'A' && ch <= 'Z') {
      //ch = (ch & 0x1F) - 1; -- This might not fly across platforms -- Huan.
      ch -= 'A';
	} else if (ch >= 'a' && ch <= 'z') { 
	  ch -= 'a';
    } else if (ch >= '2' && ch <= '7') {
      //ch -= '2' - 26; -- Will this work if '2' < 26? -- Huan.
	  ch = ch - '2' + 26;
    } else {
      return -1;
    }

    buffer |= ch;
    bitsLeft += 5;
    if (bitsLeft >= 8) {
      result[count++] = buffer >> (bitsLeft - 8);
      bitsLeft -= 8;
    }
  }
  if (count < bufSize) {
    result[count] = '\000';
  }
  return count;
}






struct otp_cache sOtp_cache;

void sx_otp(u8 line)
{
  sOtp_cache.disp_scroll = (sOtp_cache.disp_scroll + 1) % 2;
  sOtp_cache.needs_screen_updated = 1;
}


void update_otp(u8 line, u8 update)
{
  if (sOtp_cache.needs_screen_updated) {
	// display_chars(switch_seg(line, LCD_SEG_L1_3_0, LCD_SEG_L2_4_0), itoa(sOtp_cache.disp_scroll == 0 ? sOtp_cache.last_hash / 10 : sOtp_cache.last_hash % 100000, 5, 0), SEG_ON);
	
	// The screen is kind of wacky and can only display 5 chars at once.
	// So we need to display
	if (sOtp_cache.disp_scroll == 0) {
	  display_chars(switch_seg(line, LCD_SEG_L1_3_0, LCD_SEG_L2_4_0), (u8*)" -XXX", SEG_ON);
	  display_chars(switch_seg(line, LCD_SEG_L1_3_0, LCD_SEG_L2_2_0), itoa(sOtp_cache.last_hash / 1000, 3, 0), SEG_ON);
	} else {
	  display_chars(switch_seg(line, LCD_SEG_L1_3_0, LCD_SEG_L2_4_0), itoa(sOtp_cache.last_hash % 1000, 3, 0), SEG_ON);
	  display_chars(switch_seg(line, LCD_SEG_L1_3_0, LCD_SEG_L2_1_0), (u8*)"- ", SEG_ON);
	}
	
	sOtp_cache.needs_screen_updated = 0;
  }
  
  // If this is not the 30th second, never bother updating
  // Unless we have never updated the totp
  if (sTime.system_time % 30 && sOtp_cache.last_update) return;
  
  // If the token was calculated in the last 15 sec, never bother updating
  // Note that the above return won't cover this one
  // As the timer ticks 100 times a sec
  if ((sTime.system_time - sOtp_cache.last_update) < 15) return;
  
  sOtp_cache.disp_scroll = 0;
  u8 buf[] = GOOGLE_SECRET_KEY; 
  // Don't worry, this is not my personal account key.
  u8 buf_len = 17;
  u8 *secret = buf;
  
  u8 secret_len;
  secret_len = base32_decode(secret, secret, buf_len);
  // display_chars(LCD_SEG_L1_3_0, (u8*)"TOTP", SEG_ON);
  // display_chars(LCD_SEG_L2_4_0, itoa((sTime.system_time + SYSTEM_EPOCH_OFFSET) % 100000, 5, 0), SEG_ON);
  
  
  u32 tm = (sTime.system_time+SYSTEM_EPOCH_OFFSET)/30;
  u16 i = 1;
  //for (i = -1; i <= 1; ++i) {
    u8 challenge[8];
    u32 chlg = tm + i;
	u32 j;
	
    for (j = 8; j--; chlg >>= 8) {
      challenge[j] = chlg;
    }
    
    u8 hash[SHA1_DIGEST_LENGTH];
    hmac_sha1(secret, secret_len, challenge, 8, hash, SHA1_DIGEST_LENGTH);

	u32 offset = hash[SHA1_DIGEST_LENGTH - 1] & 0xF;
    u32 truncatedHash = 0;
    for (j = 0; j < 4; ++j) {
      truncatedHash <<= 8;
      truncatedHash  |= hash[offset + j];
    }
    
    memset(hash, 0, sizeof(hash));
    truncatedHash &= 0x7FFFFFFF;
    truncatedHash %= 1000000;
	sOtp_cache.last_hash = truncatedHash;
	sOtp_cache.needs_screen_updated = 1;
	sOtp_cache.last_update = sTime.system_time;
	//display_chars(LCD_SEG_L1_3_0, itoa(truncatedHash / 100000, 4, 0), SEG_ON);
    //display_chars(switch_seg(line, LCD_SEG_L1_3_0, LCD_SEG_L2_4_0), itoa(truncatedHash / 10, 5, 0), SEG_ON);
	//pru16f("%d\n",truncatedHash);
  //}
}

void display_otp(u8 line, u8 update)
{
	if (update == DISPLAY_LINE_UPDATE_FULL)
	{
		display_chars(switch_seg(line, LCD_SEG_L1_3_0, LCD_SEG_L2_4_0), (u8 *)"  totp", SEG_ON);
	}
}




#endif // CONFIG_OTP