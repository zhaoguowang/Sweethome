#include "socket_utils.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <string>


extern void do_tcp_test(int);

int main(int argc, char* argv[]) {
    do_tcp_test(100000);
    return 0;
}
