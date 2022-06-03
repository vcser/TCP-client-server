#ifndef CRYPTO_H
#define CRYPTO_H

#include <sodium.h>

struct crypto_context {
    unsigned char k[crypto_secretstream_xchacha20poly1305_KEYBYTES];
    unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    crypto_secretstream_xchacha20poly1305_state state;
};

void send_encrypted_file(struct crypto_context *context, int sock, const char *path, int file_size);
void recv_encrypted_file(struct crypto_context *context, int sock, const char *path, int file_size);

#endif
