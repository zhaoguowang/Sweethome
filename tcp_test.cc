#include "socket_utils.h"
#include "date_time.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <string>

#define _GNU_SOURCE
#include <sched.h>

const char* local_addr = "216.165.108.106:1393";
// const char* local_addr = "127.0.0.1:1393";

#define L1_Cache_Size (32*1024)
#define L2_Cache_Size (256*1024)

inline int flush_cache(uint32_t size)
{
   int8_t *cache = new int8_t[size];
   for(int i = 0; i < size; i ++) {
        cache[i] = i;
   }

   int res = 0;
   for(int i = 0; i < size; i ++) {
        res += cache[i];
   }

   delete[] cache;

   return res;
}

inline void bind_cpu(uint cpu)
{
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(cpu, &cpuset);
  int s = sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
  if (s != 0)
      fprintf(stderr, "pthread_setaffinity_np");

}


bool start_server(int n_rounds, int work_load)
{

    bind_cpu(1);

    int ss = msg::socket_utils::tcp_listen(local_addr);
    printf("[Server] Listen On Sock %d\n", ss);
    struct sockaddr so_addr;
    ::memset(&so_addr, 0, sizeof(so_addr));
    socklen_t so_addrlen = 0;
    
    int clnt = ::accept(ss, &so_addr, &so_addrlen);

    char *data = new char[work_load];
    printf("[Server] Connection Sock %d\n", clnt);

    uint64_t total_cycles = 0;
    uint64_t write_cycles = 0;
    uint64_t read_cycles = 0;

    uint64_t start = Time::read_tsc();

    int r = n_rounds;

    while(r > 0) {

        uint64_t r_start = Time::read_tsc();

        int rs = ::read(clnt, data, work_load);
        
        read_cycles += Time::read_tsc() - r_start;

        uint64_t w_start = Time::read_tsc();

        int ws = ::write(clnt, data, work_load);

        write_cycles += Time::read_tsc() - w_start;

        r--;
    }

    total_cycles = Time::read_tsc() - start;

    printf("======================================== [Server] Profile Data ==========================================\n");
    printf("[Server] total time (ms) %f\n", Time::cycles_to_ms(total_cycles));
    printf("[Server] per req time (ms) %f\n", Time::cycles_to_ms(total_cycles/n_rounds));
    printf("[Server] recv time (ms) %f\n", Time::cycles_to_ms(read_cycles/n_rounds));
    printf("[Server] send time (ms) %f\n", Time::cycles_to_ms(write_cycles/n_rounds));

    //A barriar to wait for server finish
    char y = 'y';
    int rs = ::read(clnt, &y, 1);
    int ws = ::write(clnt, &y, 1);
    // fprintf(stderr, "ws %d rs %d\n", ws, rs);

    ::close(ss);

    return true;
}



bool start_client(int n_rounds, int work_load)
{

    bind_cpu(5);

    int sock = 0;
    do {
        sock = msg::socket_utils::tcp_connect(local_addr);
    } while(sock < 0);

    printf("[Client] Listen On Sock %d\n", sock);

    uint64_t total_cycles = 0;
    uint64_t write_cycles = 0;
    uint64_t read_cycles = 0;

    char *data = new char[work_load];

    uint64_t start = Time::read_tsc();

    int r = n_rounds;

    while(r > 0) {
        
        flush_cache(L2_Cache_Size);

        uint64_t w_start = Time::read_tsc();

        int ws = ::write(sock, data, work_load);

        write_cycles += Time::read_tsc() - w_start;

        uint64_t r_start = Time::read_tsc();

        int rs = ::read(sock, data, work_load);

        read_cycles += Time::read_tsc() - r_start;

        r--;
    }

    total_cycles = Time::read_tsc() - start;

    //A barriar to wait for server finish
    char x = 'x';
    int ws = ::write(sock, &x, 1);
    int rs = ::read(sock, &x, 1);

    // fprintf(stderr, "ws %d rs %d\n", ws, rs);

    printf("======================================== [Client] Profile Data ==========================================\n");
    printf("[Client] total time (ms) %f\n", Time::cycles_to_ms(total_cycles));
    printf("[Client] per req time (ms) %f\n", Time::cycles_to_ms(total_cycles/n_rounds));
    printf("[Client] send time (ms) %f\n", Time::cycles_to_ms(write_cycles/n_rounds));
    printf("[Client] recv time (ms) %f\n", Time::cycles_to_ms(read_cycles/n_rounds));
    
    ::close(sock);

    return true;
}

void do_tcp_test(int n_rounds, int work_load)
{
    fprintf(stderr, "==================== TCP Test Rounds [%d] WorkLoads [%d]======================\n", n_rounds, work_load);
    pid_t pid = fork();

    if(pid < 0) {
        fprintf(stderr, "Create Process Failed\n");
        exit(1);
    }

    if(pid == 0) {
        start_server(n_rounds, work_load);
    } else {
        start_client(n_rounds, work_load);
    }

}
