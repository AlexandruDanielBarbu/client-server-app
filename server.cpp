#include <iostream>
#include <cstring>
#include <vector>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "cli_checks_utils.h"
#include "connect_utils.h"
#include "epoll_manipulation_utils.h"
#include "alex_simple_protocol.h"
#include "communication_utils.h"

#define CONNECTED_MESSAGE_FORMAT "New client %d connected from %s:%d."
#define DISCONNECT_MESSAGE_FORMAT "Client %d disconnected."

#define MAX_CONNECTIONS 100
#define MAX_EPOLL_ARRAY_SIZE (MAX_CONNECTIONS + 3)

using namespace std;

int main(int argc, char const *argv[]) {
    // Input checks
    if (argc != 2 ||                    // cli arguments check, accept only 2
        !is_port_number(argv[1])) {     // port is a number check
        cerr << "Number of arguments or arguments invalid!\n";
        return 1;
    }
    
    int port_number;
    sscanf(argv[1], "%d", &port_number);

    int udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket_fd == -1) {
        cerr << "UDP socket could not be oppened!\n";
        return 1;
    }
    
    int tcp_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_listen_fd == -1) {
        cerr << "TCP socket could not be oppened!\n";
        return 1;
    }
    
    server_bind(port_number, udp_socket_fd);

    server_bind(port_number, tcp_listen_fd);
    int max_connections_misto = 100;
    listen(tcp_listen_fd, max_connections_misto);

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        cerr << "Error on epoll fd open\n";
        return 1;
    }
    
    struct epoll_event udp_event, tcp_event, stdin_event;
    config_epoll_event(udp_event, epoll_fd, udp_socket_fd);
    config_epoll_event(tcp_event, epoll_fd, tcp_listen_fd);
    config_epoll_event(stdin_event, epoll_fd, STDIN_FILENO);
    
    vector<struct epoll_event> events_array(MAX_EPOLL_ARRAY_SIZE);

    while (true) {
        int n = epoll_wait(epoll_fd, events_array.data(), MAX_EPOLL_ARRAY_SIZE, -1);
        if (n == -1) {
            cerr << "Error at epoll_wait!\n";
            return 1;
        }

        for (int i = 0; i < n; i++)
        {
            int fd = events_array[i].data.fd;

            if (fd == udp_socket_fd) {
                // TODO Get message from udp client
                // TODO Apply algorithm to detect potential users
                // TODO Send to correct user the message 
            } else if (fd == tcp_listen_fd) {
                // Create new socket for new connection
                int s = accept(tcp_listen_fd, NULL, NULL);
                // monitor this link for a change
                struct epoll_event client_event;
                config_epoll_event(client_event, epoll_fd, s);
            } else if (fd == STDIN_FILENO) {
                char client_message[10000];
                fgets(client_message, 10000, stdin);
                char cmd[20];
                sscanf(client_message, "%s", cmd);
                if (strcmp(cmd, "exit") == 0) {
                    printf("EXITING...\n");
                    goto my_exit;
                } else {
                    printf("unknown command!\n");
                }
            } else {
                // Client sends me something
                asp_header client_response;
                client_response = accept_request(fd);
                printf("OP: %d\nID: %d\nIP: %s\nPORT: %d\nTOPIC: %s", client_response.operation, client_response.client_id,
                client_response.ip_address, client_response.port, client_response.topic);

                // TODO EXIT
                // TODO Subscribe
                // TODO Unsubscirbe

            }
        }
    }

my_exit:
    printf("This is the end of the server!\n");
    return 0;
}
