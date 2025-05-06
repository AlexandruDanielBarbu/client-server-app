#ifndef CONNECT_UTILS
#define CONNECT_UTILS

int server_bind(int port_number, int fd);
int client_connect(const char *server_ip,
                   const int port_number,
                   const int socket_fd);
int shutdown_and_close(int fd);

#endif  // CONNECT_UTILS