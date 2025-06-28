#ifndef Socket_H
#define Socket_H

#include <sys/socket.h>    // for socket(), bind(), etc.

class Socket {
private:
    int socket_fd;

public:
    Socket();
    Socket(int socket_fd);
    ~Socket();

    int get_fd();

    bool create_socket(const int domain, const int type, const int protocol);
    bool set_socket_opt(const int level, const int option_name,
                        const void *option_value, const socklen_t option_len);

    // Used by server
    bool bind_socket(const int port_number);
    bool listen_socket(const int max_connections);

    // Used by client
    bool connect_socket(const char *server_ip,
                        const int port_number);
};

#endif  // Socket_H
