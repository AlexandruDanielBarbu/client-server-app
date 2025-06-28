#include "connect_utils.h"

#include <unistd.h>        // for close(), shutdown()
#include <netinet/in.h>    // for sockaddr_in, htons(), htonl()
#include <arpa/inet.h>     // for inet_pton(), inet_ntoa(), etc. (optional, but useful)

int shutdown_and_close(int fd) {
    shutdown(fd, SHUT_RDWR);
    close(fd);
    
    return 0;
}
