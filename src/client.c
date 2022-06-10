#include <sodium.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "crypto.h"
#include "tcp.h"
#include "util.h"

int main(int argc, const char *argv[]) {
    // verificar que los parametros cli esten correctos
    if (argc < 3) {
        printf("usage: %s <ip> <file> [options]", argv[0]);
        exit(0);
    }
    const char *ip = argv[1];
    const char *file_name = argv[2];
    const char *key_file = "key";
    int port = DEFAULTPORT;
    char flags = 0;

    // parsear los parametros cli
    for (int i = 3; i < argc; i++) {
        const char *option = argv[i];
        if (option[0] != '-')
            continue;
        if (option[1] == 'u')
            flags |= UNENCRYPTED;
        else if (option[1] == 'k') {
            key_file = argv[i + 1];
        } else if (option[1] == 'p') {
            port = atoi(argv[i + 1]);
        }
    }

    // perdon por usar goto
    if (flags & UNENCRYPTED)
        goto NOENCRYPTION;

    // inicializar libsodium
    if (sodium_init() != 0) {
        // error
        fprintf(stderr, "Error: could not initialize libsodium\n");
        exit(EXIT_FAILURE);
    }

    struct crypto_context context;
    // leer el archivo con la clave secreta
    FILE *key = fopen(key_file, "rb");
    if (key == NULL) {
        fprintf(stderr, "Error: could not read key file\n");
        exit(EXIT_FAILURE);
    }
    fread(context.k, 1, crypto_secretstream_xchacha20poly1305_KEYBYTES, key);
    fclose(key);

NOENCRYPTION:;
    int file_size = fsize(file_name);

    struct tcp_client_t server;
    // conectarse al sevidor en la ip y puerto seleccionados
    tcp_client_connect(&server, ip, port);
    // enviar el tamaño del nombre del archivo
    tcp_send_size(server.sock, strlen(argv[2]) + 1);
    // enviar el nombre del archivo
    tcp_send(server.sock, file_name, strlen(argv[2]) + 1);
    // decirle al cliente si los datos enviados estaran encriptados
    tcp_send(server.sock, &flags, 1);
    // enviar el tamaño del archivo
    tcp_send_size(server.sock, file_size);

    // imprimir info del archivo que se esta enviando
    printf("Sending...\n");
    printf("file name: %s\n", file_name);
    printf("file size: %d bytes\n", file_size);

    // enviar el contenido del archivo
    if (flags & UNENCRYPTED)
        tcp_send_file(server.sock, file_name, file_size);
    else
        send_encrypted_file(&context, server.sock, file_name, file_size);

    tcp_close(server.sock);

    return 0;
}
