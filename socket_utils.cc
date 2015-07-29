// General utilities
// Yang Zhang <y@yzhang.net>, 2014
// See LICENSE file for copyright notice

#include "socket_utils.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <netinet/tcp.h>

#include <errno.h>


using std::string;

namespace msg {

int socket_utils::create_tcp_socket(
        const std::string& addr,
        const struct addrinfo* hints,
        socket_filter_t filter /* =? */,
        struct sockaddr** p_addr /* =? */,
        socklen_t* p_len /* =? */) {

    int sock = -1;
    size_t idx = addr.find(":");
    if (idx == string::npos) {
        return -1;
    }
    string host = addr.substr(0, idx);
    string port = addr.substr(idx + 1);

    struct addrinfo *result = nullptr, *rp;
    int r = ::getaddrinfo((host == "0.0.0.0") ? nullptr : host.c_str(),
                          port.c_str(),
                          hints,
                          &result);
    if (r != 0) {
        return -1;
    }

    for (rp = result; rp != nullptr; rp = rp->ai_next) {
        sock = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock == -1) {
            continue;
        } else if (filter != nullptr &&
                   filter(sock, rp->ai_addr, rp->ai_addrlen) == false) {
            ::close(sock);
            sock = -1;
            continue;
        } else {
            break;  // Found!
        }
    }

    if (rp == nullptr || sock == -1) {
        return -1;

    } else if (p_addr != nullptr && p_len != nullptr) {
        *p_addr = static_cast<struct sockaddr *>(malloc(rp->ai_addrlen));
        *p_len = rp->ai_addrlen;
        memcpy(*p_addr, rp->ai_addr, *p_len);
    }

    return sock;
}

int socket_utils::tcp_listen(const std::string& addr) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;  // TCP
    hints.ai_flags = AI_PASSIVE;      // server side

    socket_filter_t filter =
        [] (int sock, const struct sockaddr* so_addr, socklen_t so_addrlen) {
            const int y = 1;
            setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(y));
            setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &y, sizeof(y));
            int ret = ::bind(sock, so_addr, so_addrlen);
            ::perror(::strerror(errno));
            return ret == 0;
        };

    int sock = create_tcp_socket(addr, &hints, filter);
    if (sock > 0) {
        // about backlog: http://www.linuxjournal.com/files/linuxjournal.com/
        //                linuxjournal/articles/023/2333/2333s2.html
        const int backlog = SOMAXCONN;
        ::listen(sock, backlog) == 0;
    }
    return sock;
}

int socket_utils::tcp_connect(const std::string& addr) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;  // TCP

    socket_filter_t filter =
        [] (int sock, const struct sockaddr* so_addr, socklen_t so_addrlen) {
            int ret = ::connect(sock, so_addr, so_addrlen);
            ::perror(::strerror(errno));
            return ret == 0;
        };

    int sock = create_tcp_socket(addr, &hints, filter);
    if (sock > 0) {
        const int y = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(y));
        setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &y, sizeof(y));
    }
    return sock;
}

}  // namespace msg
