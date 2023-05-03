#ifndef AES256gcm_H
#define AES256gcm_H

#include <stddef.h>
#include <stdint.h>
#include "../include/util.h"
#define AES256gcm_BLOCKBYTES    64
#define AES256gcm_HASHBYTES     16
#define AES256gcm_NAMESPACE(s)  libESP_aes256gcm_##s

typedef struct {
  uint64_t sk_exp[120];
  uint32_t ivw[16];
} aes256gcm_ctx;
//***********************************************user structure def*********************************************************************//
typedef struct {
    uint8_t  key[32];
    uint8_t  nonce[12];
    uint8_t* stream_key;
}aes256gcm_key_set;

typedef struct {
    uint8_t  Ghash[AES256gcm_HASHBYTES];
    uint8_t  enc_tag_result[AES256gcm_HASHBYTES];
    uint8_t  dec_tag_result[AES256gcm_HASHBYTES];

    uint8_t* ad; //additional data
    size_t   ad_size;
}aes256gcm_ghash_set;


#define aes256gcm_prf AES256gcm_NAMESPACE(_prf)
void aes256gcm_prf(uint8_t *out, size_t outlen, const uint8_t key[32], const uint8_t nonce[12]);

#define aes256gcm_init AES256gcm_NAMESPACE(_init)
void aes256gcm_init(aes256gcm_ctx* state, const uint8_t key[32], const uint8_t nonce[12]);

#define aes256gcm_squeezeblocks AES256gcm_NAMESPACE(_squeezeblocks)
void aes256gcm_squeezeblocks(uint8_t *out, size_t outLen, aes256gcm_ctx *state);
//***********************************************user function def*********************************************************************//
#define aes256gcm_encrypt AES256gcm_NAMESPACE(_encrypt)
void aes256gcm_encrypt(uint8_t* inputText, uint8_t* stream_key, size_t encLen, aes256gcm_ghash_set* ghash_set);
#define aes256gcm_decrypt AES256gcm_NAMESPACE(_decrypt)
int aes256gcm_decrypt(uint8_t* inputText, uint8_t* stream_key, size_t declen, aes256gcm_ghash_set* ghash_set);
#endif
