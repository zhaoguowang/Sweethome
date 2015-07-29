#pragma once

// General utilities
// Yang Zhang <y@yzhang.net>, 2014
// See LICENSE file for copyright notice

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <functional>
#include <string> 
namespace msg {

class socket_utils {
public:
    // input: socket file descriptor, socket address, address length
    // output: true -> accept, false -> reject
    typedef std::function<bool(int, const struct sockaddr*, socklen_t)>
        socket_filter_t;

    // returned p_addr should be free()'d
    static int create_tcp_socket(
        const std::string& addr,
        const struct addrinfo* hints,
        socket_filter_t filter = nullptr,
        struct sockaddr** p_addr = nullptr,
        socklen_t* p_len = nullptr);

    static int tcp_listen(const std::string& addr);
    static int tcp_connect(const std::string& addr);
};

}  // namespace msg
