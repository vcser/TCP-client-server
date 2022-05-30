#ifndef TCP_H
#define TCP_H

#include <netinet/in.h>

// https://cl1lib.org/book/829755/45845b

// Servidor
struct tcp_server_t {
    struct sockaddr_in server_addr;
    int listen_sock;
};

// Crea el servidor TCP
void tcp_server_create(struct tcp_server_t *server, int port);

// Acepta una conexión TCP (crea un socket para comunicarse con el primer
// cliente en la fila)
int tcp_server_accept(struct tcp_server_t *server,
                      struct sockaddr_in *client_addr,
                      socklen_t *client_addr_len);

// Cliente
struct tcp_client_t {
    struct sockaddr_in server_addr;
    int sock;
};

// Crea una conexión TCP con el servidor
void tcp_client_connect(struct tcp_client_t *client, const char *host,
                        int port);

// Común

// Envía y recibe datos
void tcp_send(int sock, const void *data, size_t size);
void tcp_recv(int sock, void *data, size_t size);

// Cierra la conexión
void tcp_close(int sock);

#endif  // TCP_H
