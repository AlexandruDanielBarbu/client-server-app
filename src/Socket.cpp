#include "Socket.h"

#include <cstring>         // for std::memset
#include <unistd.h>        // for close(), shutdown()
#include <netinet/in.h>    // for sockaddr_in, htons(), htonl()
#include <arpa/inet.h>     // for inet_pton(), inet_ntoa(), etc. (optional, but useful)
// #include <cerrno>          // for errno
#include <iostream>        // for std::cerr


// Create an empty Socket object
Socket::Socket() {
    socket_fd = -1;
}

// Or directly asign it a value
Socket::Socket(int socket_fd) {
    this->socket_fd = socket_fd;
}

// On deallocation shutdown and close the connection
Socket::~Socket() {
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);
}

int Socket::get_fd() {
    return socket_fd;
}

bool Socket::create_socket(const int domain, const int type, const int protocol) {
    socket_fd = socket(domain, type, protocol);

    if (socket_fd == -1) return false;
    return true;
}

bool Socket::set_socket_opt(const int level, const int option_name,
                        const void *option_value, const socklen_t option_len) {
    int rc = setsockopt(socket_fd, level, option_name, option_value, option_len);

    if (rc == -1) return false;
    return true;
}

bool Socket::bind_socket(const int port_number) {
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port_number);

    int rc = bind(socket_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

    if (rc == -1) return false;
    return true;
}

bool Socket::listen_socket(const int max_connections) {
    int rc = listen(socket_fd, max_connections);

    if (rc == -1) return false;
    return true;
}

bool Socket::connect_socket(const char *server_ip,
                        const int port_number) {

    struct sockaddr_in addr;

    // Zero out the addr
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, server_ip, &addr.sin_addr);
    addr.sin_port = htons(port_number);

    // Try to connect the client
    int rc = connect(socket_fd,
                    (struct sockaddr *)&addr,
                     sizeof(struct sockaddr_in));

    if (rc == -1) return false;
    return true;
}