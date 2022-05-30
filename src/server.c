#include <stdio.h>

#include "tcp.h"

int main(void) {
    struct tcp_server_t server;

    tcp_server_create(&server, 10000);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len;
        int sock = tcp_server_accept(&server, &client_addr, &client_addr_len);

        printf("Connected\n");

        tcp_send(sock, "hola", 5);

        char buf[256];
        tcp_recv(sock, buf, 5);
        printf("%s\n", buf);

        tcp_close(sock);
        break;
    }

    return 0;
}
