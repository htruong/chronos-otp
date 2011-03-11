/*
 * otp.h
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

#ifndef OTP_H_
#define OTP_H_

#define SHA1_BLOCKSIZE 64
#define SHA1_DIGEST_SIZE 20
#define OTP_KEY_LEN 10

// Extern section
extern void sx_otp(u8 line);
extern void display_otp(u8 line, u8 update);
extern void update_otp(u8 line, u8 update);

typedef struct {
    u32 state[5];
    u32 count[2];
    u8  buffer[SHA1_BLOCKSIZE];
} SHA1_CTX;

struct otp_cache {
  u32 last_hash;
  u32 last_update;
  u8 disp_scroll;
  u8 needs_screen_updated;
  u8 secretkey[OTP_KEY_LEN];
};

extern struct otp_cache sOtp_cache;

void SHA1_Init(SHA1_CTX* context);
void SHA1_Update(SHA1_CTX* context, const u8* data, const u32 len);
void SHA1_Final(SHA1_CTX* context, u8 digest[SHA1_DIGEST_SIZE]);

#endif /*otp_H_*/