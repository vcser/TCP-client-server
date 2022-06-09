#include "tcp.h"
#include "util.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sodium.h>

#define BACKLOG 5

void tcp_server_create(struct tcp_server_t *server, int port) {
    // Crear socket de escucha (se guarda en server->listen_sock)
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        // error
        fprintf(stderr, "Error: could not create server socket\n");
        exit(EXIT_FAILURE);
    }
    server->listen_sock = fd;
    // Bindear socket a puerto
    server->server_addr.sin_family = AF_INET;
    server->server_addr.sin_port = htons(port);
    server->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(fd, (struct sockaddr *)&server->server_addr, sizeof(server->server_addr)) == -1) {
        // error
        fprintf(stderr, "Error: could not bind server socket\n");
        exit(EXIT_FAILURE);
    }
    // Escuchar conexiones entrantes
    if (listen(fd, BACKLOG) == -1) {
        // error
        fprintf(stderr, "Error: cannot listen on server socket\n");
        exit(EXIT_FAILURE);
    }
}

int tcp_server_accept(struct tcp_server_t *server,
                      struct sockaddr_in *client_addr,
                      socklen_t *client_addr_len) {
    // Aceptar primera conexion entrante (guardar dirección del cliente en
    // client_addr)
    int fd = accept(server->listen_sock, (struct sockaddr *)client_addr, client_addr_len);
    if (fd == -1) {
        fprintf(stderr, "Error: could not accept connection\n");
        exit(EXIT_FAILURE);
    }
    // Retorna descriptor de archivo del socket de comunicación con el cliente
    return fd;
}

void tcp_client_connect(struct tcp_client_t *client, const char *host,
                        int port) {
    // Crear socket de cliente (se guarda en client->sock)
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        // error
        fprintf(stderr, "Error: could create client socket\n");
        exit(EXIT_FAILURE);
    }
    client->sock = fd;
    client->server_addr.sin_family = AF_INET;
    client->server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &client->server_addr.sin_addr) != 1) {
        // error
        fprintf(stderr, "Error: could not resolve address\n");
        exit(EXIT_FAILURE);
    }
    // Conectar con host y puerto indicados (se guarda en client->server_addr y
    // se usa en llamada a connect())
    if (connect(fd, (struct sockaddr *)&client->server_addr, sizeof(client->server_addr)) == -1) {
        // error
        fprintf(stderr, "Error: could not connect to server\n");
        exit(EXIT_FAILURE);
    }
}

void tcp_send(int sock, const void *data, size_t size) {
    write(sock, data, size);
}

void tcp_recv(int sock, void *data, size_t size) {
    read(sock, data, size);
}

void tcp_send_size(int sock, uint32_t n) {
    uint32_t size = htonl(n);
    write(sock, (uint8_t *)&size, 4);
}

uint32_t tcp_recv_size(int sock) {
    uint8_t buff[4];
    read(sock, buff, 4);
    uint32_t size = *(uint32_t *)buff;
    size = ntohl(size);
    return size;
}

void tcp_send_file(int sock, const char *path, int size) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: could not open file %s\n", path);
        exit(EXIT_FAILURE);
    }
    char buff[STEP] = {0};
    int n = min(size, STEP), max_size = size + n, len = -n;
    do {
        max_size -= n;
        len += n;
        int progress = len * 100 / size;
        progress_bar(progress);
        fread(buff, sizeof(char), n, file);
    } while ((n = write(sock, buff, min(STEP, max_size))) > 0);
    fclose(file);
    printf("\n");
}

void tcp_recv_file(int sock, const char *path, int size) {
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: could not open file %s\n", path);
        exit(EXIT_FAILURE);
    }
    char buff[STEP];
    int n, max_size = size, len = 0;
    while ((n = read(sock, buff, min(STEP, max_size))) > 0) {
        max_size -= n;
        len += n;
        int progress = len * 100 / size;
        progress_bar(progress);
        fwrite(buff, sizeof(char), n, file);
    }
    fclose(file);
    printf("\n");
}

void tcp_close(int sock) {
    close(sock);
}
