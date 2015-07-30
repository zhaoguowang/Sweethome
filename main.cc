#include "socket_utils.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <iostream>

extern void do_tcp_test(int, int);

int main(int argc, char* argv[]) {


    std::string bench_type = "tcp";
    int n_round = 10000;
    int workload = 64;

    optind = 1;

    while (1) {

        static struct option long_options[] =
        {
            {"bench" ,  required_argument , 0, 'b'},
            {"n_round" ,    required_argument , 0, 'n'},
            {"workload" ,   required_argument , 0, 'w'},
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "b:n:w", long_options, &option_index);

        if (c == -1)
          break;

        switch (c)
        {
            case 0:
            if (long_options[option_index].flag != 0)
              break;
            abort();
            break;

            case 'b':
            bench_type = optarg;
            std::cout<<"run bench "<< bench_type <<std::endl;
            break;

            case 'n':
            n_round  = atoi(optarg);    
            std::cout<<"rounds "<< n_round<<std::endl;
            break;

            case 'w':
            workload  = atoi(optarg);    
            std::cout<<"workload "<< workload <<std::endl;
            break;

            default:
              fprintf(stderr, "Wrong Arg %d\n", c);
              exit(1);
        }
     
    }

    if(bench_type == "tcp")
        do_tcp_test(n_round, workload);

    return 0;
}
