#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sodium.h>

#include "tcp.h"
#include "crypto.h"
#include "util.h"

int main(int argc, char *argv[]) {
    const char *key_file = "key";
    int port = DEFAULTPORT;
    char flags = 0;

    // parsear los parametros cli
    for (int i = 1; i < argc; i++) {
        const char *option = argv[i];
        if (option[0] != '-') continue;
        if (option[1] == 'k') {
            key_file = argv[i + 1];
        }
        else if (option[1] == 'p') {
            port = atoi(argv[i + 1]);
        }
    }


    // crear servidor
    struct tcp_server_t server;
    tcp_server_create(&server, port);
    printf("starting server at port %d\n", port);

    while (1) {
        // aceptar la conexion con el cliente
        struct sockaddr_in client_addr;
        socklen_t client_addr_len;
        int sock = tcp_server_accept(&server, &client_addr, &client_addr_len);

        printf("Connected\n");

        // recibir el tamaño del nombre del archivo
        int name_size = tcp_recv_size(sock);

        // recibir el nombre del archivo
        char file_name[name_size];
        tcp_recv(sock, file_name, name_size);

        // saber si el archivo recibido esta encriptado
        tcp_recv(sock, &flags, 1);
        if (flags & UNENCRYPTED) {
            printf("recibiendo archivo encriptado\n");
            goto NOENCRYPTION;
        } else printf("recibiendo archivo encriptado\n");

        // inicializar libsodium
        if (sodium_init() != 0) {
            // error
            fprintf(stderr, "Error: could not initialize libsodium\n");
            exit(EXIT_FAILURE);
        }
        struct crypto_context context;

        // leer archivo con la llave secreta
        FILE *key = fopen("key", "rb");
        if (key == NULL) {
            fprintf(stderr, "Error: could not read key file\n");
            exit(EXIT_FAILURE);
        }
        fread(context.k, 1, crypto_secretstream_xchacha20poly1305_KEYBYTES, key);
        fclose(key);

NOENCRYPTION:;
        // recibir el tamaño del archivo
        int file_size = tcp_recv_size(sock);

        // imprimir informacion del archivo que se esta recibiendo
        printf("Recieving...\n");
        printf("file name: %s\n", file_name);
        printf("file size: %d\n", file_size);

        // recibir los contenidos del archivo
        if (flags & UNENCRYPTED) tcp_recv_file(sock, file_name, file_size);
        else recv_encrypted_file(&context, sock, file_name, file_size);

        tcp_close(sock);
        break;
    }

    return 0;
}
