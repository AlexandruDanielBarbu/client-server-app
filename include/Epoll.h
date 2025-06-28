#ifndef Epoll_H
#define Epoll_H

#include <vector>

class Epoll {
private:
    int epoll_fd;

public:
    Epoll();
    ~Epoll();

    int get_fd();
    bool create_epoll();
    bool config_epoll_event(struct epoll_event& my_event,
                            const int fd);
    int wait_for_events(std::vector<struct epoll_event>& events,
                            const int maxevents, const int timeout = -1);
};
#endif  // Epoll_H