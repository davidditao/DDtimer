// 双向升序链表实现定时容器
#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <functional>

class util_timer;

/* 定时器链表。它是一个升序、双向链表，且带有头节点和尾节点 */
class sort_timer_lst
{
public:
    sort_timer_lst() : head(nullptr), tail(nullptr) {}
    void add_timer(util_timer *timer);    // 添加定时器
    void adjust_timer(util_timer *timer); // 调整定时器时间
    void del_timer(util_timer *timer);    // 删除定时器
    void tick();                          // 处理定时器事件

private:
    void add_timer(util_timer *timer, util_timer *lst_head); // 辅助函数

    util_timer *head; // 头节点
    util_timer *tail; // 尾节点
};
