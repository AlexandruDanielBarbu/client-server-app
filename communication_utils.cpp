#include "communication_utils.h"

#include <sys/socket.h>
#include <sys/types.h>

using namespace std;

int recv_all(int sockfd, void *buffer, size_t len) {
    size_t bytes_received = 0;
    size_t bytes_remaining = len;
    char *buff = (char *)buffer;

    while(bytes_remaining) {
        int rc = recv(sockfd, buff, bytes_remaining, 0);
        if (rc == -1)
            return -1;

        bytes_received += rc;
        bytes_remaining -= len;        
    }

    return bytes_received;
}

int send_all(int sockfd, void *buffer, size_t len) {
    size_t bytes_sent = 0;
    size_t bytes_remaining = len;
    char *buff = (char *) buffer;

    while(bytes_remaining) {
        int rc = send(sockfd, buff, bytes_remaining, 0);
        if (rc == -1)
            return rc;

        bytes_sent += rc;
        bytes_remaining -= rc;        
    }

    return bytes_sent;
}
