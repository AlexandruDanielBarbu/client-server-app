#ifndef EPOLL_MANIPULATION_UTILS
#define EPOLL_MANIPULATION_UTILS

int config_epoll_event(struct epoll_event& my_event,
                       const int epoll_fd,
                       const int fd);


#endif  // EPOLL_MANIPULATION_UTILS