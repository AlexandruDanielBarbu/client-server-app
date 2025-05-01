#include "alex_simple_protocol.h"

#include <string>
#include <cstring>

#include "communication_utils.h"

using namespace std;

asp_header build_header(const int op, const int client_id, const char *ip_address, const int port, const char *topic) {
    asp_header rez;
    rez.operation = op;
    rez.client_id = client_id;
    rez.ip_address = strdup(ip_address);
    rez.port = port;
    rez.topic = strdup(topic);

    return rez;
}

size_t get_data_syze(asp_header& data) {
    size_t offset = 0;
    offset += sizeof(int);
    offset += sizeof(int);
    offset += sizeof(int);

    offset += strlen(data.ip_address) * sizeof(char);
    offset += strlen(data.topic) * sizeof(char);

    return offset;
}

void send_request(asp_header& data, const int sockfd) {
    char buffer[BUFFER_MAX_LEN];
    int offset = 0;
    // Compute sending buffer
    size_t data_size = get_data_syze(data);
    memcpy(buffer + offset, &data_size, sizeof(size_t));
    offset += sizeof(size_t);

    memcpy(buffer + offset, &data.operation, sizeof(int));
    offset += sizeof(int);

    memcpy(buffer + offset, &data.client_id, sizeof(int));
    offset += sizeof(int);
    
    memcpy(buffer + offset, data.ip_address, strlen(data.ip_address) * sizeof(char));
    offset += strlen(data.ip_address) * sizeof(char);
    
    memcpy(buffer + offset, &data.port, sizeof(int));
    offset += sizeof(int);

    memcpy(buffer + offset, data.topic, strlen(data.topic) * sizeof(char));
    offset += strlen(data.topic) * sizeof(char);
    
    // Send buffer
    send_all(sockfd, buffer, offset);

    // Free allocated memory
    free(data.ip_address);
    free(data.topic);
}

// Returns a struct that has the read values of the protocol, strings are allocated
asp_header accept_request(const int sockfd) {
    size_t data_size = 0;
    recv_all(sockfd, (void *)&data_size, sizeof(size_t));

    asp_header rez;
    char *buffer = (char *)calloc(data_size, sizeof(char));

    rez.ip_address = (char *)calloc(20, sizeof(char));
    rez.topic = (char *) calloc(10000,sizeof(char));
    recv_all(sockfd, (void *)buffer, data_size);

    sscanf(buffer, "%d%d%s%d%s", &rez.operation, &rez.client_id, rez.ip_address, &rez.port, rez.topic);

    free(buffer);
    return rez;
}
