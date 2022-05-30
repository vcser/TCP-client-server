#include "tcp.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define BACKLOG 5
#define SERVER_PORT 50500

void tcp_server_create(struct tcp_server_t *server, int port) {
    // Crear socket de escucha (se guarda en server->listen_sock)
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        // error
    }
    server->listen_sock = fd;
    // Bindear socket a puerto
    server->server_addr.sin_family = AF_INET;
    server->server_addr.sin_port = htons(SERVER_PORT);
    server->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(fd, (struct sockaddr *)&server->server_addr, sizeof(server->server_addr)) == -1) {
        // error
    }
    // Escuchar conexiones entrantes
    if (listen(fd, BACKLOG) == -1) {
        // error
    }
}

int tcp_server_accept(struct tcp_server_t *server,
                      struct sockaddr_in *client_addr,
                      socklen_t *client_addr_len) {
    // Aceptar primera conexion entrante (guardar dirección del cliente en
    // client_addr)
    int fd = accept(server->listen_sock, (struct sockaddr *)client_addr, client_addr_len);
    if (fd == -1) {
        // error
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
    }
    client->sock = fd;
    client->server_addr.sin_family = AF_INET;
    client->server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, host, &client->server_addr.sin_addr) != 1) {
        // error
    }
    // Conectar con host y puerto indicados (se guarda en client->server_addr y
    // se usa en llamada a connect())
    if (connect(fd, (struct sockaddr *)client->server_addr, sizeof(client->server_addr)) == -1) {
        // error
    }
}

void tcp_send(int sock, const void *data, size_t size) {
    write(sock, data, size);
}

void tcp_recv(int sock, void *data, size_t size) {
    read(sock, data, size);
}

void tcp_close(int sock) {
    close(sock);
}
