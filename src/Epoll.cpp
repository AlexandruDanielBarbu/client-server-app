#include "Epoll.h"

#include <sys/epoll.h>
#include <vector>
#include <unistd.h>        // for close(), shutdown()

Epoll::Epoll() {
    epoll_fd = -1;
}

Epoll::~Epoll() {
    close(epoll_fd);
}

int Epoll::get_fd() {
    return epoll_fd;
}

bool Epoll::create_epoll() {
    epoll_fd = epoll_create1(0);

    if (epoll_fd == -1) return false;
    return true;
}

bool Epoll::config_epoll_event(struct epoll_event& my_event,
                                const int fd) {
    my_event.events = EPOLLIN;
    my_event.data.fd = fd;

    int rc = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &my_event);

    if ( rc == -1) return false;
    return true;
}

int Epoll::wait_for_events(std::vector<struct epoll_event>& events,
                            const int maxevents, const int timeout) {
   int n = epoll_wait(epoll_fd, events.data(), maxevents, timeout);

   return n;
}