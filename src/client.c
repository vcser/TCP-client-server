#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tcp.h"

int main(int argc, const char *argv[]) {
    if (argc < 3) {
        printf("usage: %s <ip> <file>", argv[0]);
        exit(0);
    }
    const char *ip = argv[1];
    const char *file_name = argv[2];

    FILE *f = fopen(argv[2], "r");
    // get file size
    fseek(f, 0, SEEK_END);
    int file_size = ftell(f);
    rewind(f);

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
    tcp_send_file(server.sock, f, file_size);

    tcp_close(server.sock);
    fclose(f);

    return 0;
}
