#include <iostream>
#include <cstring>
#include <vector>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <unordered_map>
#include <arpa/inet.h>
#include <math.h>
#include <sstream>
#include <netinet/tcp.h>

#include "cli_checks_utils.h"
#include "connect_utils.h"
#include "epoll_manipulation_utils.h"
#include "alex_simple_protocol.h"
#include "communication_utils.h"
#include "node.h"
#include "topic_tree.h"

// UDP constants
constexpr int MAX_UDP_MESSAGE_LEN    = 51 + sizeof(char) + 1501;

// TCP constants
constexpr int MAX_CONNECTIONS        = 100;
constexpr int MAX_EPOLL_ARRAY_SIZE   = MAX_CONNECTIONS + 3;
constexpr int MAX_CLIENT_MESSAGE_LEN = 80;

// Alex Simple Protocol constants
constexpr int MAX_CLIENT_ID_LEN      = 11;
constexpr int MAX_IP_ADDR_STR_LEN    = 16;
constexpr int MAX_TOPIC_LEN          = 51;

#include "Socket.h"
#include "Epoll.h"

int main(int argc, char const *argv[]) {
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);

    // Input checks
    if (argc != 2 ||                    // cli arguments check, accept only 2
        !is_port_number(argv[1])) {     // port is a number check
        std::cerr << "Number of arguments or arguments invalid!\n";
        return 1;
    }
    
    int port_number;
    sscanf(argv[1], "%d", &port_number);

    int yes = 1;

    unordered_map<string, Client> clients_hash_map;
    Node* topic_tree_root = new Node("");

    // UDP client conection
    Socket my_udp_socket;
    if (!my_udp_socket.create_socket(AF_INET, SOCK_DGRAM, 0) ||
        !my_udp_socket.set_socket_opt(SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) ||
        !my_udp_socket.bind_socket(port_number)) return 1;

    // TCP client connection
    Socket my_tcp_socket;
    if (!my_tcp_socket.create_socket(AF_INET, SOCK_STREAM, 0) ||
        !my_tcp_socket.set_socket_opt(SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) ||
        !my_tcp_socket.set_socket_opt(IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) ||
        !my_tcp_socket.bind_socket(port_number) ||
        !my_tcp_socket.listen_socket(MAX_CONNECTIONS)) return 1;

    // Epoll configuration
    Epoll epoll_instance;
    if (!epoll_instance.create_epoll()) return 1;

    struct epoll_event udp_event, tcp_event, stdin_event;
    epoll_instance.config_epoll_event(udp_event, my_udp_socket.get_fd());
    epoll_instance.config_epoll_event(tcp_event, my_tcp_socket.get_fd());
    epoll_instance.config_epoll_event(stdin_event, STDIN_FILENO);

    int events_array_capacity = MAX_EPOLL_ARRAY_SIZE;
    vector<struct epoll_event> events_array(events_array_capacity);
    bool will_resize = false;

    struct sockaddr_in udp_client_addr;
    socklen_t addr_len = sizeof(udp_client_addr);

    while (true) {
        if (will_resize) {
            events_array.resize(events_array_capacity * 2);
            will_resize = false;
        }
        
        int n = epoll_instance.wait_for_events(events_array, MAX_EPOLL_ARRAY_SIZE);
        if (n == -1) return 1;
        if (n > events_array_capacity / 2) will_resize = true;
        
        for (int i = 0; i < n; i++) {
            int fd = events_array[i].data.fd;

            if (fd == my_udp_socket.get_fd()) {
                // Get message from udp client
                char udp_buffer[MAX_UDP_MESSAGE_LEN];
                ssize_t len = recvfrom(my_udp_socket.get_fd(), udp_buffer, MAX_UDP_MESSAGE_LEN, 0,
                                      (struct sockaddr *)&udp_client_addr, &addr_len);
                if (len < 0) goto my_fail_exit;

                udp_buffer[len] = '\0';

                string notification; /* = string(inet_ntoa(udp_client_addr.sin_addr)) + ":" +
                                      to_string(ntohs(udp_client_addr.sin_port)) + " - ";
                                      */

                char topic[MAX_TOPIC_LEN] = {0};
                std::memcpy(topic, udp_buffer, 50);
                unsigned char tip = *(udp_buffer + 50);
                
                notification += string(udp_buffer) + " - ";

                switch (tip) {
                case 0:
                    {
                        notification += "INT - ";
                        char c = *(udp_buffer + 50 + sizeof(char));
                        int n;
                        memcpy(&n, udp_buffer + 50 + sizeof(char) + sizeof(char), sizeof(int));
                        n = ntohl(n);

                        if (c != 0 && n != 0) {
                            notification += "-";
                        }
                        notification += to_string(n);
                    }
                    break;
                
                case 1:
                    {
                        notification += "SHORT_REAL - ";
                        uint16_t short_val;
                        memcpy(&short_val, udp_buffer + 50 + sizeof(char), sizeof(uint16_t));
                        short_val = ntohs(short_val);

                        float val = short_val / 100.0f;

                        char final_val[32];
                        snprintf(final_val, sizeof(final_val), "%.2f", val);
                        notification += final_val;
                    }
                    break;
                case 2:
                    {
                        notification += "FLOAT - ";

                        unsigned char sign;
                        uint32_t raw_value;
                        unsigned char power;

                        size_t offset = 50 + sizeof(uint8_t);

                        memcpy(&sign, udp_buffer + offset, sizeof(unsigned char));
                        offset += sizeof(unsigned char);

                        memcpy(&raw_value, udp_buffer + offset, sizeof(uint32_t));
                        raw_value = ntohl(raw_value);
                        offset += sizeof(uint32_t);

                        memcpy(&power, udp_buffer + offset, sizeof(unsigned char));

                        double float_value = raw_value / pow(10.0, power);
                        if (sign) float_value *= -1;

                        notification += to_string(float_value);
                    }
                    break;
                case 3:
                    {
                        notification += "STRING - ";

                        size_t offset = 50 + sizeof(uint8_t);
                        const char* str_start = udp_buffer + offset;

                        notification += string(str_start);
                    }
                    break;
                
                default:
                    break;
                }
                
                std::vector<std::string> tokens;
                std::string intermediate;
                std::stringstream check1(topic);

                while (std::getline(check1, intermediate, '/'))
                    tokens.push_back(std::move(intermediate));

                // Apply algorithm to detect potential users
                // Send to correct user the message
                notify_clients(topic_tree_root, tokens, tokens.begin(), notification);
            } else if (fd == my_tcp_socket.get_fd()) {  // pot spune cu certitudine ca merge
                // Create new socket for new connection
                int s = accept(my_tcp_socket.get_fd(), NULL, NULL);
                if (s == -1)
                    continue;
                
                string data = receive_data(s);
                if (data == "EROARE DE ZILE MARI!" ||
                    data == "SOCKET INCHIS GRACEFULLY") {
                    close(s);
                    continue;
                }
                
                int op;
                sscanf(data.c_str(), "%d", &op);

                if (op == ASP_CLIENT_CONNECT) {
                    int dead;
                    char client_id[MAX_CLIENT_ID_LEN];
                    char client_ip[MAX_IP_ADDR_STR_LEN];
                    int client_port;

                    sscanf(data.c_str(), "%d%s%s%d", &dead, client_id, client_ip, &client_port);

                    Client incoming_client = Client(string(client_id), client_port, string(client_ip));
                    
                    if (clients_hash_map.find(incoming_client.get_id()) == clients_hash_map.end()) {
                        // Client not found
                        int result = setsockopt(s,
                            IPPROTO_TCP,
                            TCP_NODELAY,
                            (char *) &yes, 
                            sizeof(int));
                        if(result == -1) {
                            goto my_fail_exit;
                        }
                        // Monitor this link for a change
                        struct epoll_event client_event;
                        epoll_instance.config_epoll_event(client_event, s);
                        incoming_client.set_client_fd(s);

                        // Inserting client into map
                        clients_hash_map[incoming_client.get_id()] = incoming_client;
                        std::cout << "New client " + incoming_client.get_id() +
                                " connected from " + incoming_client.get_client_ip() + ":" +
                                to_string(incoming_client.get_client_port_number()) << endl;
                    } else {
                        // Client found
                        send_command_resp(ASP_SEND_FAIL, s, data);
                        close(s);

                        std::cout << "Client " << incoming_client.get_id() << " already connected." << endl;
                    }
                }
            } else if (fd == STDIN_FILENO) {
                char client_message[MAX_CLIENT_MESSAGE_LEN];
                fgets(client_message, MAX_CLIENT_MESSAGE_LEN, stdin);
                
                char cmd[20];
                sscanf(client_message, "%s", cmd);

                if (strcmp(cmd, "exit") == 0) {
                    // Unsubscribe clients and close connections
                    for (auto it : clients_hash_map) {
                        it.second.unsubscribe_client_from_all_topics();
                        send_quit(it.second.get_client_fd());
                        shutdown_and_close(it.second.get_client_fd());
                    }
                    goto my_exit;
                } else {
                    // printf("Some other command :): \n%s\n", client_message);
                }
            } else {
                // Client sends me something
                string data = receive_data(fd);
                if (data == "EROARE DE ZILE MARI!" ||
                    data == "SOCKET INCHIS GRACEFULLY") {
                    // Client disconnected
                    // Hopefully i don't get here
                    continue;
                }
                
                int op;
                sscanf(data.c_str(), "%d", &op);

                switch (op) {
                case ASP_CLIENT_EXIT:
                    // EXIT for client
                    {
                        int dead;
                        char client_id[MAX_CLIENT_ID_LEN];
                        sscanf(data.c_str(), "%d%s", &dead, client_id);
                        string cl_id(client_id);

                        clients_hash_map[cl_id].unsubscribe_client_from_all_topics();
                        int rc = send_command_resp(ASP_SEND_SUCCESS, clients_hash_map[cl_id].get_client_fd(), data);
                        (void)rc;
                        // if (rc == -1) {
                        //     // client is disconnected
                        // }
                        
                        // Terminate connection with client
                        shutdown_and_close(clients_hash_map[cl_id].get_client_fd());
                        clients_hash_map.erase(cl_id);

                        std::cout << "Client " << cl_id << " disconnected." << endl;
                    }
                    break;

                case ASP_CLIENT_SUBSCRIBE:
                    {
                        int dead;
                        char topic[MAX_TOPIC_LEN];
                        char client_id[MAX_CLIENT_ID_LEN];
                        sscanf(data.c_str(), "%d%s%s", &dead, topic, client_id);
                        
                        string cl_id(client_id);
                        string tpc(topic);

                        int rez = add_path_to_tree(topic_tree_root, &clients_hash_map[cl_id], tpc);
                        if (rez == 0) {
                            send_command_resp(ASP_SEND_SUCCESS, clients_hash_map[cl_id].get_client_fd(),  data);
                        } else {
                            send_command_resp(ASP_SEND_FAIL, clients_hash_map[cl_id].get_client_fd(),  data);
                        }
                    }
                    break;                
                
                case ASP_CLIENT_UNSUBSCRIBE:
                    {
                        int dead;
                        char topic[MAX_TOPIC_LEN];
                        char client_id[MAX_CLIENT_ID_LEN];
                        sscanf(data.c_str(), "%d%s%s", &dead, topic, client_id);

                        string cl_id(client_id);
                        string tpc(topic);

                        int rez = clients_hash_map[cl_id].unsubscribe_client_from_topic(topic_tree_root, tpc);
                        if (rez == 0) {
                            send_command_resp(ASP_SEND_SUCCESS, clients_hash_map[cl_id].get_client_fd(),  data);
                        } else {
                            send_command_resp(ASP_SEND_FAIL, clients_hash_map[cl_id].get_client_fd(),  data);
                        }
                    }
                    break;

                default:
                    break;
                }
            }
        }
    }

// Attempt to gracefull exit :)
my_exit:
    topic_tree_cleanup(topic_tree_root);
    return 0;

my_fail_exit:
    topic_tree_cleanup(topic_tree_root);
    return 1;
}
