#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sodium.h>

#include "tcp.h"
#include "crypto.h"

int main(int argc, const char *argv[]) {
    // crypto struff
    if (sodium_init() != 0) {
        // error
        fprintf(stderr, "Error: could not initialize libsodium\n");
        exit(EXIT_FAILURE);
    }

    struct crypto_context context;
    // read key file
    FILE *key = fopen("key", "rb");
    if (key == NULL) {
        fprintf(stderr, "Error: could not read key file\n");
        exit(EXIT_FAILURE);
    }
    fread(context.k, 1, crypto_secretstream_xchacha20poly1305_KEYBYTES, key);
    fclose(key);


    if (argc < 3) {
        printf("usage: %s <ip> <file>", argv[0]);
        exit(0);
    }
    const char *ip = argv[1];
    const char *file_name = argv[2];

    FILE *f = fopen(argv[2], "r");
    if (f == NULL) {
        fprintf(stderr, "Error: could not find the file %s\n", file_name);
        exit(EXIT_FAILURE);
    }
    // get file size
    fseek(f, 0, SEEK_END);
    int file_size = ftell(f);
    rewind(f);
    fclose(f);

    struct tcp_client_t server;

    // connect to server at ip:port
    tcp_client_connect(&server, ip, PORT);

    // send file name size
    tcp_send_size(server.sock, strlen(argv[2]) + 1);
    
    // send file name
    tcp_send(server.sock, file_name, strlen(argv[2])+1);
    
    // send file size
    tcp_send_size(server.sock, file_size);

    // print file info
    printf("Sending...\n");
    printf("file name: %s\n", file_name);
    printf("file size: %d\n", file_size);

    // send file content
    // tcp_send_file(server.sock, file_name, file_size);
    send_encrypted_file(&context, server.sock, file_name, file_size);

    tcp_close(server.sock);

    return 0;
}
