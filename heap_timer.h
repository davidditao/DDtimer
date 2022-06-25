#pragma once

#include <netinet/in.h>
#include <time.h>
#include <functional>

#define BUFFER_SIZE 64

class heap_timer;

struct client_data
{
    sockaddr_in address;
    int sockfd;
    char buff[BUFFER_SIZE];
    heap_timer *timer;
};

class heap_timer
{
    using cb = std::function<void(client_data *data)>;

public:
    heap_timer(int delay)
    {
        expire = time(NULL) + delay;
    }

public:
    time_t expire; // 定时器生效的绝对时间
    cb cb_func;
    client_data *user_data;
};