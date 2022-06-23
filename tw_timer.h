#pragma once
#include <netinet/in.h>
#include <functional>

#define BUFFER_SIZE 64

class tw_timer;

/* 用户数据结构：绑定 socket 和 定时器 */
struct client_data
{
    sockaddr_in address;    // 客户端地址
    int sockfd;             // 客户端socket
    char *buf[BUFFER_SIZE]; // 读缓存
    tw_timer *timer;        // 定时器
};

/* 定时器类：至少要包含两个成员，超时时间和任务回调函数 */
class tw_timer
{
    using callback = std::function<void(client_data *data)>;

public:
    tw_timer(int rot, int ts)
        : rotation(rot), time_slot(ts), prev(nullptr), next(nullptr) {}

public:
    int rotation;           // 记录定时器在时间轮转过多少圈后生效
    int time_slot;          // 记录定时器属于时间轮上的那个槽
    client_data *user_data; // 客户数据
    callback cb_func;       // 定时器回调函数
    tw_timer *prev;         // 指向前一个定时器
    tw_timer *next;         // 指向后一个定时器
};