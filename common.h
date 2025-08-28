#include <stddef.h>
#ifndef COMMON_H
#define COMMON_H

#include <cotp.h>
#include <openssl/rand.h>

#define MAX_OTP 255
#define SECRETS_DIR ".c2fa/"

struct OTP {
    char *secret;
    char *filename;
    unsigned int digits:4;
    unsigned int algo:2;
    unsigned int period_short:1;
    unsigned int locked:1;
};

extern struct OTP OTPs[MAX_OTP];

struct OTP* create_otp(const char *secret, const char *filename, int digits, char algo);
int create_secrets(char **secret, cotp_error_t *err);
void secure_wipe_otp(struct OTP *otp); 
#endif
