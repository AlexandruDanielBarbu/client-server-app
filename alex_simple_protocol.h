#ifndef ALEX_SIMPLE_PROTOCOL
#define ALEX_SIMPLE_PROTOCOL


#define ASP_EXIT 0
#define ASP_SUBSCRIBE 1
#define ASP_UNSUBSCRIBE 2

#define BUFFER_MAX_LEN 1000000

struct asp_header {
    int operation;
    int client_id;
    char *ip_address;
    int port;
    char *topic;
};

asp_header build_header(const int op, const int client_id, const char *ip_address, const int port, const char *topic);
void send_request(asp_header& data, const int sockfd);
asp_header accept_request(const int sockfd);

#endif  // ALEX_SIMPLE_PROTOCOL