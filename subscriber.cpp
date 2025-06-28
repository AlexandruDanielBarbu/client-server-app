#include <iostream>
#include <cstring>
#include <vector>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <netinet/tcp.h>

#include "cli_checks_utils.h"
#include "connect_utils.h"
#include "alex_simple_protocol.h"
#include "client.h"

#define SUBSCRIBE_FORMAT_MESSAGE "Subscribed to topic "
#define UNSUBSCRIBE_FORMAT_MESSAGE "Unsubscribed from topic "

// Epoll constants
constexpr int MAX_EPOLL_ARRAY_SIZE   = 102;
constexpr int MAX_CLIENT_MESSAGE_LEN = 80;

// Topic tree constants
constexpr int MAX_TOPIC_LEN          = 51;
constexpr int MAX_UDP_MESSAGE_LEN    = (51 + sizeof(int) + 1501);

#include "Epoll.h"
#include "Socket.h"

int main(int argc, char const *argv[]) {
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);
    
    // Input checks
    if (argc != 4 ||
        !is_ip_address(argv[2]) ||
        !is_port_number(argv[3])) {
        std::cerr << "Please use executable correctly\n";
        return 1;
    }
    
    int port_number;
    sscanf(argv[3], "%d", &port_number);

    int yes = 1;

    Client myself = Client(string(argv[1]), port_number, string(argv[2]));

    // Client socket setup
    Socket my_socket;
    if (!my_socket.create_socket(AF_INET, SOCK_STREAM, 0) ||
        !my_socket.set_socket_opt(IPPROTO_TCP, TCP_NODELAY, (char *) &yes, sizeof(int)) ||
        !my_socket.connect_socket(argv[2], port_number)) return 1;

    send_connect_request(myself, my_socket.get_fd());
    
    Epoll epoll_instance;
    if (!epoll_instance.create_epoll()) return 1;

    struct epoll_event tcp_event, stdin_event;
    epoll_instance.config_epoll_event(tcp_event, my_socket.get_fd());
    epoll_instance.config_epoll_event(stdin_event, STDIN_FILENO);

    int events_array_capacity = MAX_EPOLL_ARRAY_SIZE;
    vector<struct epoll_event> events_array(events_array_capacity);
    bool will_resize = false;

    while (true) {
        if (will_resize) {
            // if vector is close to full, double in size
            events_array.resize(events_array_capacity * 2);
            will_resize = false; 
        }
        
        int n = epoll_instance.wait_for_events(events_array, events_array_capacity);
        if (n == -1) {
            std::cerr << "Error at epoll_wait!\n";
            goto my_fail_exit;
        }
        if (n > events_array_capacity / 2)
            will_resize = true;
        
        for (int i = 0; i < n; i++) {
            int fd = events_array[i].data.fd;

            if (fd == my_socket.get_fd()) {
                // Handle TCP message
                // got response from server
                string data = receive_data(my_socket.get_fd());
                if (data == "EROARE DE ZILE MARI!" ||
                    data == "SOCKET INCHIS GRACEFULLY")
                    goto my_exit;
                
                int op;
                sscanf(data.c_str(), "%d", &op);

                switch (op) {
                case ASP_SEND_FAIL:
                    {
                        int dead;
                        int op2;
                        sscanf(data.c_str(), "%d%d", &dead, &op2);

                        if (op2 == ASP_CLIENT_CONNECT) {
                            // am fail pe connect
                            goto my_fail_exit;
                        }
                    }
                    break;
                
                case ASP_SEND_SUCCESS:
                    {
                        int dead;
                        int op2;
                        sscanf(data.c_str(), "%d%d", &dead, &op2);

                        switch (op2) {
                        case ASP_CLIENT_SUBSCRIBE:
                            {
                                int dead1, dead2;
                                char topic[MAX_TOPIC_LEN];
                                sscanf(data.c_str(), "%d%d%s", &dead1, &dead2, topic);
                                
                                cout << SUBSCRIBE_FORMAT_MESSAGE << topic << endl;
                            }
                            break;
                        
                        case ASP_CLIENT_UNSUBSCRIBE:
                            {
                                int dead1, dead2;
                                char topic[MAX_TOPIC_LEN];
                                sscanf(data.c_str(), "%d%d%s", &dead1, &dead2, topic);

                                cout << UNSUBSCRIBE_FORMAT_MESSAGE << topic << endl;
                            }
                            break;

                        case ASP_CLIENT_EXIT:
                            {
                                // pot sa dau exit
                                goto my_exit;
                            }
                            break;
                        
                        case ASP_CLIENT_CONNECT:
                            {
                                // pot sa dau connect
                                // las asa nu fac nimic
                            }
                            break;
                            
                        default:
                            break;
                        }
                    }
                    break;
                
                case ASP_SEND_BUFFER:
                    {
                        // The UDP data was sent succesfully
                        string output = string(data.c_str() + 2);
                        cout << output << endl;
                    }
                    break;

                case ASP_QUIT:
                    goto my_exit;
                    break;

                default:
                    break;
                }
            } else if (fd == STDIN_FILENO)
            {
                char client_message[MAX_CLIENT_MESSAGE_LEN];
                fgets(client_message, MAX_CLIENT_MESSAGE_LEN, stdin);
                
                char cmd[20];
                sscanf(client_message, "%s", cmd);

                if (strcmp(cmd, "exit") == 0) {
                    send_exit(myself, my_socket.get_fd());
                    // wait for server response
                    // goto my_exit;
                } else if (strcmp(cmd, "subscribe") == 0) {
                    char topic[MAX_TOPIC_LEN];
                    sscanf(client_message + strlen(cmd), "%s", topic);

                    send_sub_unsub(myself, ASP_CLIENT_SUBSCRIBE, string(topic), my_socket.get_fd());
                    // wait server response
                } else if (strcmp(cmd, "unsubscribe") == 0) {
                    char topic[MAX_TOPIC_LEN];
                    sscanf(client_message + strlen(cmd), "%s", topic);

                    send_sub_unsub(myself, ASP_CLIENT_UNSUBSCRIBE, string(topic), my_socket.get_fd());
                    // wait server response
                } else {
                    // printf("Some other command:\n%s\n", client_message);
                }
            }
        }
    }

my_exit:
    return 0;

my_fail_exit:
    return 1;
}
