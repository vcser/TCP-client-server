#include <sodium.h>
#include <sodium/core.h>
#include <sodium/crypto_secretstream_xchacha20poly1305.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "tcp.h"
#include "util.h"
#include "crypto.h"

struct crypto_context crypto_init() {
    if (sodium_init() != 0) {
        // error
        fprintf(stderr, "Error: could not initialize libsodium\n");
        exit(EXIT_FAILURE);
    }
    struct crypto_context context;
    
    crypto_secretstream_xchacha20poly1305_init_push(&context.state, context.header, context.k);
    return context;
}

void crypto_genkey(const char *file) {
    FILE *f = fopen(file, "wb");
    unsigned char k[crypto_secretstream_xchacha20poly1305_KEYBYTES];
    crypto_secretstream_xchacha20poly1305_keygen(k);
    fwrite(k, 1, crypto_secretstream_xchacha20poly1305_KEYBYTES, f); 
    fclose(f);
}

unsigned long long crypto_encrypt_stream(unsigned char *buff, size_t size, struct crypto_context *context, unsigned char tag) {
    unsigned char buff_out[STEP + crypto_secretstream_xchacha20poly1305_ABYTES];
    unsigned long long out_len;

    crypto_secretstream_xchacha20poly1305_push(&context->state, buff_out, &out_len, buff, STEP, NULL, 0, tag);
    
    return out_len;
}

void send_encrypted_file(struct crypto_context *context, int sock, const char *path, int file_size) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: could not open file %s\n", path);
        exit(EXIT_FAILURE);
    }
    unsigned char buf_in[STEP] = {0};
    unsigned char buf_out[STEP + crypto_secretstream_xchacha20poly1305_ABYTES], tag;
    //unsigned long long out_len;
    int n = min(file_size, STEP), max_size = file_size + n, len = -n;
    if (crypto_secretstream_xchacha20poly1305_init_push(&context->state, context->header, context->k) == -1) {
        fprintf(stderr, "Error: could not init push\n");
        exit(EXIT_FAILURE);
    }
    write(sock, context->header, sizeof(context->header));
    do {
        max_size -= n;
        len += n;
        int progress = len * 100 / file_size;
        progress_bar(progress);
        fread(buf_in, sizeof(char), n, file);
        tag = feof(file) ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;
        if (crypto_secretstream_xchacha20poly1305_push(&context->state, buf_out, NULL, buf_in, min(STEP, max_size), NULL, 0, tag) == -1) {
            fprintf(stderr, "Error: could not push buffer\n"); 
            exit(EXIT_FAILURE);
        }
    } while ((n = write(sock, buf_out, min(STEP, max_size) + crypto_secretstream_xchacha20poly1305_ABYTES) - crypto_secretstream_xchacha20poly1305_ABYTES) > 0);
    fclose(file);
    printf("\n");
}

// probando funcion para dejar mas limpio el codigo
unsigned char *encrypt_buffer(struct crypto_context *context, unsigned char *buf_in, unsigned int buf_len, unsigned char tag) {
    unsigned char *buf_out = malloc(buf_len + crypto_secretstream_xchacha20poly1305_ABYTES);
    
    if (crypto_secretstream_xchacha20poly1305_push(&context->state, buf_out, NULL, buf_in, buf_len, NULL, 0, tag) == -1) {
        fprintf(stderr, "Error: could not push buffer\n"); 
        exit(EXIT_FAILURE);
    }

    return buf_out;
}

void recv_encrypted_file(struct crypto_context *context, int sock, const char *path, int file_size) {
    FILE *file = fopen("file-SERVER", "w");
    if (file == NULL) {
        fprintf(stderr, "Error: could not open file %s\n", path);
        exit(EXIT_FAILURE);
    }
    unsigned char buf_in[STEP + crypto_secretstream_xchacha20poly1305_ABYTES];
    unsigned char buf_out[STEP];
    unsigned char tag;
    unsigned long long n;
    int max_size = file_size, len = 0;
    read(sock, context->header, sizeof(context->header));
    if (crypto_secretstream_xchacha20poly1305_init_pull(&context->state, context->header, context->k) == -1) {
        fprintf(stderr, "Error: bad crypto header\n");
        exit(0);
    }
    while ((n = read(sock, buf_in, min(STEP, max_size) + crypto_secretstream_xchacha20poly1305_ABYTES)) > 0) {
        if (crypto_secretstream_xchacha20poly1305_pull(&context->state, buf_out, &n, &tag, buf_in, n, NULL, 0) == -1) {
            printf("n: %lld\n", n);
            fprintf(stderr, "Error: corrupt buffer\n");
            exit(EXIT_FAILURE);
        }
        max_size -= n;
        len += n;
        int progress = len * 100 / file_size;
        progress_bar(progress);
        fwrite(buf_out, sizeof(char), n, file);
    }
    fclose(file);
    printf("\n");
}


