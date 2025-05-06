#include <iostream>
#include <cstring>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "epoll_manipulation_utils.h"

using namespace std;

int config_epoll_event(struct epoll_event& my_event,
                       const int epoll_fd,
                       const int fd) {
    my_event.events = EPOLLIN;
    my_event.data.fd = fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &my_event) == -1) {
        cerr << "Error on my_event epoll_ctl.\n";
        return -1;
    }

    return 0;
}