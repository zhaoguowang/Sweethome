#include "socket_utils.h"
#include "date_time.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <string>

const char* local_addr = "216.165.108.106:1393";

bool start_server(int n_rounds)
{
    int ss = msg::socket_utils::tcp_listen(local_addr);
    printf("[Server] Listen On Sock %d\n", ss);
    struct sockaddr so_addr;
    ::memset(&so_addr, 0, sizeof(so_addr));
    socklen_t so_addrlen = 0;
    
    int clnt = ::accept(ss, &so_addr, &so_addrlen);
    printf("[Server] Connection Sock %d\n", clnt);

    while(n_rounds > 0) {

        char b = 'y';

        ::read(clnt, &b, 1);
        
        ::write(clnt, &b, 1);

        n_rounds--;
    }
    
    ::close(ss);

    return true;
}



bool start_client(int n_rounds)
{
    int sock = 0;
    do {
        sock = msg::socket_utils::tcp_connect(local_addr);
    } while(sock < 0);

    printf("[Client] Listen On Sock %d\n", sock);

    uint64_t total_cycles = 0;
    uint64_t write_cycles = 0;
    uint64_t read_cycles = 0;

    uint64_t start = Time::read_tsc();

    int r = n_rounds;

    while(r > 0) {

        char b = 'x';
        
        uint64_t w_start = Time::read_tsc();

        ::write(sock, &b, 1);

        write_cycles += Time::read_tsc() - w_start;

        uint64_t r_start = Time::read_tsc();

        ::read(sock, &b, 1);

        read_cycles += Time::read_tsc() - r_start;

        r--;
    }

    total_cycles = Time::read_tsc() - start;

    printf("======================================== [Client] Profile Data ==========================================\n");
    printf("total time (ms) %f\n", Time::cycles_to_ms(total_cycles));
    printf("per req time (ms) %f\n", Time::cycles_to_ms(total_cycles/n_rounds));
    printf("send time (ms) %f\n", Time::cycles_to_ms(write_cycles/n_rounds));
    printf("recv time (ms) %f\n", Time::cycles_to_ms(read_cycles/n_rounds));
    
    ::close(sock);

    return true;
}

void do_tcp_test(int n_rounds)
{
    pid_t pid = fork();

    if(pid < 0) {
        fprintf(stderr, "Create Process Failed\n");
        exit(1);
    }

    if(pid == 0) {
        start_server(n_rounds);
    } else {
        start_client(n_rounds);
    }

}
