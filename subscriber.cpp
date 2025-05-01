#include <iostream>
#include <cstring>
#include <vector>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "cli_checks_utils.h"
#include "connect_utils.h"
#include "epoll_manipulation_utils.h"
#include "alex_simple_protocol.h"

#define SUBSCRIBE_FORMAT_MESSAGE "Subscribed to topic %s"
#define UNSUBSCRIBE_FORMAT_MESSAGE "Unsubscribed from topic %s"

#define MAX_EPOLL_ARRAY_SIZE 102

using namespace std;

struct subscriber {
    unsigned int id;
    unsigned int port_number;
    string ip_address;

    subscriber(unsigned int id, string ip, unsigned int port_number) {
        this->id = id;
        this->ip_address = ip;
        this->port_number = port_number;
    }
};

int main(int argc, char const *argv[]) {
    // Input checks
    if (argc != 4 ||
        !is_pos_number(argv[1]) ||
        !is_ip_address(argv[2]) ||
        !is_port_number(argv[3])) {
        cerr << "Please use executable correctly\n";
        return 1;
    }
    
    int port_number;
    sscanf(argv[3], "%d", &port_number);

    int client_tcp_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_tcp_socket_fd == -1) {
        cerr << "Error on client TCP socket\n";
        return 1;
    }
    
    if (client_connect(argv[2], port_number, client_tcp_socket_fd) == -1)
        return 1;
    
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        cerr << "Error on epoll fd open\n";
        return 1;
    }
    
    struct epoll_event tcp_event, stdin_event;
    config_epoll_event(tcp_event, epoll_fd, client_tcp_socket_fd);
    config_epoll_event(stdin_event, epoll_fd, STDIN_FILENO);

    vector<struct epoll_event> events_array(MAX_EPOLL_ARRAY_SIZE);

    while (true)
    {
        int n = epoll_wait(epoll_fd, events_array.data(), MAX_EPOLL_ARRAY_SIZE, -1);
        if (n == -1) {
            cerr << "Error at epoll_wait!\n";
            return 1;
        }

        for (int i = 0; i < n; i++)
        {
            int fd = events_array[i].data.fd;

            if (fd == client_tcp_socket_fd)
            {
                // TODO Handle TCP message
            } else if (fd == STDIN_FILENO)
            {
                char client_message[10000];
                fgets(client_message, 10000, stdin);
                char cmd[20];
                sscanf(client_message, "%s", cmd);

                if (strcmp(cmd, "exit") == 0)
                {
                    printf("EXITING...\n");
                    asp_header exit_msg = build_header(ASP_EXIT, atoi(argv[1]), argv[2], atoi(argv[3]), "");
                    send_request(exit_msg, client_tcp_socket_fd);
                    goto my_exit;
                } else if (strcmp(cmd, "subscribe") == 0) {
                    printf("SUBSCRIBING...\n");
                    char topic[100000];
                    char *p = client_message;

                    sscanf(p + strlen(cmd), "%s", topic);
                    asp_header subscribe_msg = build_header(ASP_SUBSCRIBE, atoi(argv[1]), argv[2], atoi(argv[3]), topic);

                    send_request(subscribe_msg, client_tcp_socket_fd);
                } else if (strcmp(cmd, "unsubscribe") == 0)
                {
                    printf("UNSUBSCRIBING...\n");
                    char topic[100000];
                    char *p = client_message;

                    sscanf(p + strlen(cmd), "%s", topic);
                    asp_header unsubscribe_msg = build_header(ASP_UNSUBSCRIBE, atoi(argv[1]), argv[2], atoi(argv[3]), topic);

                    send_request(unsubscribe_msg, client_tcp_socket_fd);
                } else {
                    printf("Some other command:\n%s\n", client_message);
                }
            }
        }
    }

my_exit:
    shutdown_and_close(client_tcp_socket_fd);
    return 0;
}
