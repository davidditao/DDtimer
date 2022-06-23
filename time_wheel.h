#pragma once

class tw_timer;

class time_wheel
{
public:
    time_wheel();
    ~time_wheel();

    tw_timer *add_timer(int timeout);     // 根据定时值 timeout 创建一个定时器，并把它插入到合适的槽中
    tw_timer *del_timer(tw_timer *timer); // 删除目标定时器 timer
    // void del_timer(tw_timer *timer);
    void tick();                          // SI时间到后，调用该函数，时间轮向前滚动一个槽的间隔
    int getSI() { return SI; }

private:
    static const int N = 60; // 时间轮上槽的数量
    static const int SI = 1; // 每 1s 转动时间轮一次，即槽间隔为 1s
    tw_timer *slots[N];      // 时间轮的槽，其中每个元素指向一个定时器链表，链表无序
    int cur_slot;            // 时间轮当前槽
};