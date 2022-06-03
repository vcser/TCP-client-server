#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sodium.h>

#include "tcp.h"
#include "crypto.h"

int main(int argc, char *argv[]) {
    // crypto stuff
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


    struct tcp_server_t server;

    tcp_server_create(&server, PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len;
        int sock = tcp_server_accept(&server, &client_addr, &client_addr_len);

        printf("Connected\n");

        // recieve file name size
        int name_size = tcp_recv_size(sock);

        // recieve file name
        char file_name[name_size];
        read(sock, file_name, name_size);

        // recieve file size
        int file_size = tcp_recv_size(sock);

        // print file info
        printf("Recieving...\n");
        printf("file name: %s\n", file_name);
        printf("file size: %d\n", file_size);

        // recieve file contents
        //tcp_recv_file(sock, file_name, file_size);
        recv_encrypted_file(&context, sock, file_name, file_size);

        tcp_close(sock);
        break;
    }

    return 0;
}
