#ifndef ALEX_SIMPLE_PROTOCOL
#define ALEX_SIMPLE_PROTOCOL

#include <string>
#include "client.h"

using namespace std;

#define ASP_SEND_FAIL           0
#define ASP_SEND_SUCCESS        1
#define ASP_SEND_BUFFER         2
#define ASP_CLIENT_EXIT         3
#define ASP_CLIENT_SUBSCRIBE    4
#define ASP_CLIENT_UNSUBSCRIBE  5
#define ASP_CLIENT_CONNECT      6
#define ASP_QUIT                7

#define BUFFER_MAX_LEN 1000000

int send_exit(Client& sending_client, int sockfd);
int send_sub_unsub(Client& sending_client, const int op, const string topic, int sockfd);
string receive_data(int sockfd);
int send_command_resp(const int op, int sockfd, string old_protocol_message);
int send_connect_request(Client& sending_client, int sockfd);
int send_response_data(std::string& data, int sockfd);
int send_quit(const int socket);

#endif  // ALEX_SIMPLE_PROTOCOL