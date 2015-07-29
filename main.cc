#include "socket_utils.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <string>

const char* local_addr = "216.165.108.107:1393";

int main(int argc, char* argv[]) {
    if (argv[1][0] == 'c') {
        // client
        int sock = msg::socket_utils::tcp_connect(local_addr);
        printf("clnt sock = %d\n", sock);
        char b = 'x';
        printf("send 1 byte: %c\n", b);
        ::write(sock, &b, 1);
        ::read(sock, &b, 1);
        printf("recv 1 byte: %c\n", b);

    } else if (argv[1][0] == 's') {
        // server
        int ss = msg::socket_utils::tcp_listen(local_addr);
        printf("server sock = %d\n", ss);
        struct sockaddr so_addr;
        ::memset(&so_addr, 0, sizeof(so_addr));
        socklen_t so_addrlen = 0;
        printf("accepting...\n");
        int clnt = ::accept(ss, &so_addr, &so_addrlen);
        printf("connection sock = %d\n", clnt);
        // read 1 byte
        char b;
        ::read(clnt, &b, 1);
        printf("got 1 byte: %c\n", b);
        b = 'y';
        printf("reply 1 byte: %c\n", b);
        // reply 1 byte
        ::write(clnt, &b, 1);
    }
    return 0;
}
