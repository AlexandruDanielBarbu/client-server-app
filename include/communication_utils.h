#ifndef COMMUNICATION_UTILS
#define COMMUNICATION_UTILS

#include <cstddef>

int recv_all(int sockfd, void *buffer, size_t len);
int send_all(int sockfd, void *buffer, size_t len);


#endif  // COMMUNICATION_UTILS