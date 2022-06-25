#pragma once

#include <vector>

class heap_timer;

class time_heap
{
public:
    time_heap() : size_(0) {}

    ~time_heap();

    void push(heap_timer *timer); // 添加定时器
    heap_timer *top();            // 获得堆顶的定时器
    void pop();                   // 弹出堆顶定时器
    void tick();                  // 心跳函数
    bool empty() const { return size_ == 0; }
    int size() const { return size_; }

private:
    void percolate_up(int child);    // 上滤操作
    void percolate_down(int parent); // 下滤操作

    std::vector<heap_timer *> array; // 堆数组(运用指针是为了开辟到堆上)
    int size_;                       // 堆数组当前包含元素的个数
};