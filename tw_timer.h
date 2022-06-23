#pragma once
#include <netinet/in.h>
#include <functional>

#define BUFFER_SIZE 64

class tw_timer;

/* 用户数据结构 */
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
private:
    client_data *user_data; // 客户数据
    callback cb; // 任务回调函数 
    tw_timer *prev; // 指向前一个定时器
    tw_timer *next; // 指向后一个定时器

};