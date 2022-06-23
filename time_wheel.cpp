#include "time_wheel.h"

#include "tw_timer.h"

time_wheel::time_wheel() : cur_slot(0)
{
    for (int i = 0; i < N; i++)
    {
        slots[i] = nullptr;
    }
}

time_wheel::~time_wheel()
{
    // 遍历每个槽，并销毁其中的定时器
    for (int i = 0; i < N; i++)
    {
        // 释放链表上的每个节点
        tw_timer *tmp = slots[i];
        while (tmp != nullptr)
        {
            slots[i] = tmp->next;
            delete tmp;
            tmp = slots[i];
        }
    }
}

// 根据定时值 timeout 创建一个定时器，并把它插入到合适的槽中
tw_timer *time_wheel::add_timer(int timeout)
{
    if (timeout < 0)
    {
        return nullptr;
    }

    int ticks = 0;
    /* 下面根据待插入定时器的超时值计算它将在时间轮转动多少次后被触发, 并将转动次数记录到变历 ticks 中。*/
    if (timeout < SI)
    {
        // 如果待插入定时器的超时值小于时间轮的槽间隔 SI, 则将 ticks 向上折合为 1
        ticks = 1;
    }
    else
    {
        // 否则将 ticks 向下折合为 timeout/SI
        ticks = timeout / SI;
    }

    int rotation = ticks / N;              // 计算待插入的定时器在时间轮传动多少圈后被触发
    int ts = (cur_slot + (ticks % N)) % N; // 计算待插入的定时器应该被插入到哪个槽中

    tw_timer *timer = new tw_timer(rotation, ts); // 创建新的定时器

    // 将定时器插入到时间轮中
    if (slots[ts] == nullptr)
    {
        printf("add timer, rotation is %d, time_slot is %d, cur_slot is %d\n",
               rotation, ts, cur_slot);
        slots[ts] = timer;
    }
    else
    {
        timer->next = slots[ts];
        slots[ts]->prev = timer;
        slots[ts] = timer;
    }
    return timer;
}

// 删除目标定时器 timer
tw_timer *time_wheel::del_timer(tw_timer *timer)
{
    if (timer == nullptr)
    {
        return nullptr;
    }

    tw_timer *tmp = timer->next;
    int ts = timer->time_slot;
    if (timer == slots[ts])
    {
        slots[ts] = slots[ts]->next;
        if (slots[ts] != nullptr)
        {
            slots[ts]->prev == nullptr;
        }
        delete timer;
    }
    else
    {
        timer->prev->next = timer->next;
        if (timer->next != nullptr)
        {
            timer->next->prev = timer->prev;
        }
        delete timer;
    }
    // 返回删除后的下一个节点
    return tmp;
}

// void time_wheel::del_timer(tw_timer *timer)
// {
//     if (timer == nullptr)
//     {
//         return;
//     }
//     int ts = timer->time_slot;
//     if (timer == slots[ts])
//     {
//         slots[ts] = slots[ts]->next;
//         if (slots[ts])
//         {
//             slots[ts]->prev = nullptr;
//         }
//         delete timer;
//     }
//     else
//     {
//         timer->prev->next = timer->next;
//         if (timer->next)
//         {
//             timer->next->prev = timer->prev;
//         }
//         delete timer;
//     }
// }

// SI时间到后，调用该函数，时间轮向前滚动一个槽的间隔
void time_wheel::tick()
{
    tw_timer *tmp = slots[cur_slot]; // 获取时间轮上当前槽的头节点
    printf("current slots is %d\n", cur_slot);
    while (tmp != nullptr)
    {
        printf("tick the timer\n");
        // 如果rotation > 0，则该定时器在这一轮不起作用
        if (tmp->rotation > 0)
        {
            tmp->rotation--;
            tmp = tmp->next;
        }
        // 否则定时器已经到期，执行定时任务，然后删除该定时器，继续遍历下一个定时器
        else
        {
            tmp->cb_func(tmp->user_data); // 执行回调
            tmp = del_timer(tmp);         // 删除该定时器
        }
        tmp = nullptr;
    }

    // 更新时间轮的当前槽
    cur_slot = (cur_slot + 1) % N;
}

// void time_wheel::tick()
// {
//     tw_timer *tmp = slots[cur_slot]; // 获取时间轮上当前槽的头节点
//     printf("current slots is %d\n", cur_slot);
//     while (tmp != nullptr)
//     {
//         printf("tick the timer\n");
//         // 如果rotation > 0，则该定时器在这一轮不起作用
//         if (tmp->rotation > 0)
//         {
//             tmp->rotation--;
//             tmp = tmp->next;
//         }
//         // 否则定时器已经到期，执行定时任务，然后删除该定时器，继续遍历下一个定时器
//         else
//         {
//             tmp->cb_func(tmp->user_data); // 执行回调
//             if (tmp == slots[cur_slot])
//             {
//                 slots[cur_slot] = tmp->next;
//                 delete tmp;
//                 if (slots[cur_slot])
//                 {
//                     slots[cur_slot]->prev = nullptr;
//                 }
//                 tmp = slots[cur_slot];
//             }
//             else
//             {
//                 tmp->prev->next = tmp->next;
//                 if (tmp->next)
//                 {
//                     tmp->next->prev = tmp->prev;
//                 }
//                 tw_timer *tmp2 = tmp->next;
//                 delete tmp;
//                 tmp = tmp2;
//             }
//         }
//     }
//
//     // 更新时间轮的当前槽
//     cur_slot = (cur_slot + 1) % N;
// }

