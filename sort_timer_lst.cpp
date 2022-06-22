#include "sort_timer_lst.h"
#include "util_timer.h"

// 添加定时器
void sort_timer_lst::add_timer(util_timer *timer)
{
    if (timer == nullptr)
    {
        return;
    }

    if (head == nullptr)
    {
        head = tail = timer;
        return;
    }

    if (timer->expire < head->expire)
    {
        // 如果直接添加到链表头
        timer->next = head;
        head->prev = timer;
        head = timer;
        printf("add the first timer!\n");
        return;
    }

    // 如果需要添加到链表中间
    add_timer(timer, head);
    printf("add a timer!\n");
}

/* 调整定时器时间：当某个定时任务发生变化时，调整对应的定时器在链表中的位置。
    这个函数只考虑被调整的定时器的超时时间延长的情况，即该定时器需要往链表的尾部移动*/
void sort_timer_lst::adjust_timer(util_timer *timer)
{
    if (timer == nullptr)
    {
        return;
    }

    util_timer *tmp = timer->next;
    if (tmp == nullptr || timer->expire < tmp->expire)
    {
        // 无需调整
        return;
    }

    // 如果该定时器的时间比后面的要大，则需要将该定时器往后移动

    if (timer == head)
    {
        // 如果该定时器是链表的头节点，直接将其取出，重新插入链表
        head = head->next;
        head->prev = nullptr;
        timer->next = nullptr;
        add_timer(timer, head);
    }
    else
    {
        // 如果该定时器在链表中间，则将其取出，插入到其原来位置之后的链表中
        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        add_timer(timer, timer->next);
    }
}

// 删除定时器
void sort_timer_lst::del_timer(util_timer *timer)
{
    if (timer == nullptr)
    {
        return;
    }

    // 如果链表中只有一个定时器
    if (timer == head && timer == tail)
    {
        delete timer;
        head = nullptr;
        tail = nullptr;
        return;
    }

    // 如果目标定时器为头节点
    if (timer == head)
    {
        head = head->next;
        head->prev = nullptr;
        delete timer;
        return;
    }

    // 如果目标定时器为尾节点
    if (timer == tail)
    {
        tail = tail->prev;
        tail->next = nullptr;
        delete timer;
        return;
    }

    // 如果目标定时器在链表中间

    timer->prev->next = timer->next;
    timer->next->prev = timer->prev;
    delete timer;
}

/* 每次定时事件触发就执行一次 tick 函数，处理链表上的到期任务 */
void sort_timer_lst::tick()
{
    if (head == nullptr)
    {
        return;
    }

    printf("timer tick\n");
    time_t cur_time = time(NULL); // 获得系统当前时间
    util_timer *tmp = head;

    // 从头节点开始一次处理每个定时器，直到遇到一个尚未到期的定时器
    while (tmp != nullptr)
    {
        if (tmp->expire > cur_time)
        {
            break;
        }

        // 调用定时器的回调函数，执行定时任务
        tmp->cb_func(tmp->user_data);
        // 执行完任务后，将它从链表中删除，并重置链表头节点
        head = tmp->next;
        if (head != nullptr)
        {
            head->prev = nullptr;
        }
        delete tmp;
        tmp = head;
    }
}

void sort_timer_lst::add_timer(util_timer *timer, util_timer *lst_head)
{
    util_timer *prev = lst_head;
    util_timer *tmp = prev->next;
    while (tmp != nullptr)
    {
        if (timer->expire < tmp->expire)
        {
            prev->next = timer;
            timer->next = tmp;
            tmp->prev = timer;
            timer->prev = prev;
            break;
        }
        prev = tmp;
        tmp = tmp->next;
    }

    // 如果插入到链表尾部，需要更新链表尾节点
    if (tmp == nullptr)
    {
        prev->next = timer;
        timer->next = nullptr;
        timer->prev = prev;
        tail = timer;
    }
}
