#include "alex_simple_protocol.h"

#include <string>
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include "communication_utils.h"
#include "client.h"

using namespace std;

int send_buffer(string& data, int sockfd) {
    int message_size = data.size();
    message_size = htonl(message_size);
    int rc = send_all(sockfd, (void *)&message_size, sizeof(int));
    if (rc == -1)
        return -1;

    rc = send_all(sockfd, (void *)data.c_str(), data.size());
    return rc;
}

int send_exit(Client& sending_client, int sockfd) {
    string buffer_to_send = to_string(ASP_CLIENT_EXIT) + " " + sending_client.serialize_client();

    return send_buffer(buffer_to_send, sockfd);
}

int send_sub_unsub(Client& sending_client, const int op, const string topic, int sockfd) {
    string buffer_to_send = to_string(op) + " " + topic + " " + sending_client.serialize_client();

    return send_buffer(buffer_to_send, sockfd);
}

int send_connect_request(Client& sending_client, int sockfd) {
    string buffer_to_send = to_string(ASP_CLIENT_CONNECT) + " " + sending_client.serialize_client();

    return send_buffer(buffer_to_send, sockfd);
}

// Will be sent to the client
int send_command_resp(const int op, int sockfd, string old_protocol_message) {
    string buffer_to_send = to_string(op) + " " + old_protocol_message;

    return send_buffer(buffer_to_send, sockfd);
}

// Will be sent to the client
int send_response_data(string& data, int sockfd) {
    string s = to_string(ASP_SEND_BUFFER) + " " + data;

    return send_buffer(s, sockfd);
}

string receive_data(int sockfd) {
    int recv_size = 0;
    int rc = recv_all(sockfd, (void *)&recv_size, sizeof(int));
    if (rc == -1) {
        return "EROARE DE ZILE MARI!";
    }
    if (rc == 0) {
        return "SOCKET INCHIS GRACEFULLY";
    }
    
    recv_size = ntohl(recv_size);
    // cout << recv_size << endl;

    char *buffer = new char[recv_size]();
    rc = recv_all(sockfd, (void *)buffer, recv_size);
    if (rc == -1)
    {
        return "EROARE DE ZILE MARI!";
    }
    if (rc == 0)
    {
        return "SOCKET INCHIS GRACEFULLY";
    }
    
    string data(buffer, recv_size);
    delete[] buffer;

    return data;
}

int send_quit(const int socket) {
    string data = to_string(ASP_QUIT);

    int  rc = send_buffer(data, socket);
    return rc;
}