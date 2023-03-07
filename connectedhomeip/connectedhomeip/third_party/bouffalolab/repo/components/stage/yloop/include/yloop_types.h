#ifndef __YLOOP_TYPES_H__
#define __YLOOP_TYPES_H__

#define POLLIN  0x1
#define POLLOUT 0x2
#define POLLERR 0x4
struct pollfd {
    int fd;  
    short events;  
    short revents;
};

#endif
