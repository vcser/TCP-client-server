#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tcp.h"

int main(void) {
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
        FILE *f = fopen("test", "w");
        tcp_recv_file(sock, f, file_size);
        fclose(f);

        tcp_close(sock);
        break;
    }

    return 0;
}
