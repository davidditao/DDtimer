#pragma once

#include <time.h>
#include <netinet/in.h>
#include <functional>

#define BUFFER_SIZE 64

class util_timer;

/* 用户数据结构 */
struct client_data
{
    sockaddr_in address;   // 客户端地址
    int sockfd;            // 客户端socket
    char buf[BUFFER_SIZE]; // 读缓存
    util_timer *timer;     // 定时器
};

/* 定时器类：至少要包含两个成员，超时时间和任务回调函数 */
class util_timer
{
    using callback = std::function<void(client_data *)>;

public:
    util_timer() : prev(nullptr), next(nullptr) {}

public:
    time_t expire;          // 任务超时时间，这里使用绝对时间
    callback cb_func;       // 任务回调函数
    client_data *user_data; // 回调函数处理的客户数据，由定时器的执行着传递给回调函数
    util_timer *prev;       // 指向前一个定时器
    util_timer *next;       // 指向后一个定时器
};
