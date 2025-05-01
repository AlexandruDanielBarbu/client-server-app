#include <iostream>
#include <cstring>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "connect_utils.h"

using namespace std;

int server_bind(int port_number, int fd) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port_number);
    
    if (bind(fd,
            (struct sockaddr *)&addr,
             sizeof(struct sockaddr_in)) == -1) {
        cerr << "Error on bind!\n";
        shutdown_and_close(fd);
        return -1;
    }

    return 0;
}

int client_connect(const char *server_ip,
                   const int port_number,
                   const int socket_fd) {
    struct sockaddr_in addr;
    
    // Zero out the addr
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, server_ip, &addr.sin_addr);
    addr.sin_port = htons(port_number);
    
    // Try to connect the client
    int rc = connect(socket_fd,
                    (struct sockaddr *)&addr,
                     sizeof(struct sockaddr_in));
    if (rc == -1) {
        // Close connection
        cerr << "Error in client connect!\n";
        shutdown_and_close(socket_fd);
        return -1;
    }

    return 0;
}

int shutdown_and_close(int fd) {
    shutdown(fd, SHUT_RDWR);
    close(fd);
    
    return 0;
}
