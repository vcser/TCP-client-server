#include <stdio.h>

#include "tcp.h"

int main(void) {
    struct tcp_client_t server;

    tcp_client_connect(&server, "localhost", 10000);

    char buf[256];
    tcp_recv(server.sock, buf, 5);
    printf("%s\n", buf);

    tcp_send(server.sock, "hola", 5);

    tcp_close(server.sock);

    return 0;
}
