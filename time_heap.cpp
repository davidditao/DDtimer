#include "time_heap.h"
#include "heap_timer.h"

#include <algorithm>

time_heap::~time_heap()
{
    for (int i = 0; i < array.size(); i++)
    {
        delete array[i];
    }
}

// 添加定时器
void time_heap::push(heap_timer *timer)
{
    if (timer == nullptr)
    {
        return;
    }
    // 先将 timer 插入到堆的最后
    array.push_back(timer);
    size_++;
    // 上滤
    percolate_up(size_ - 1);
}

// 获得堆顶的定时器
heap_timer *time_heap::top()
{
    if (size_ == 0)
    {
        return nullptr;
    }
    return array[0];
}

// 删除堆顶定时器
void time_heap::pop()
{
    if (size_ == 0)
    {
        return;
    }
    // 将堆顶元素换到最后，并删除
    std::swap(array[0], array[size_ - 1]);
    delete array[size_ - 1];
    size_--;
    // 下滤
    percolate_down(0);
}

// 心跳函数
void time_heap::tick()
{
    heap_timer *tmp = array[0];
    time_t curtime = time(NULL);
    while (!array.empty())
    {
        if (tmp == nullptr)
        {
            break;
        }

        if (tmp->expire > curtime)
        {
            // 如果堆顶定时器没到期, 退出循环
            break;
        }

        if (array[0]->cb_func != nullptr)
        {
            array[0]->cb_func(array[0]->user_data);
        }

        // 删除堆顶元素
        pop();
        tmp = array[0];
    }
}

// 上滤操作
void time_heap::percolate_up(int child)
{
    int parent = (child - 1) / 2; // child 节点的父节点
    while (child > 0)             // 注意:不需要到第一个节点
    {
        if (array[parent]->expire <= array[child]->expire)
        {
            // 如果已经放在合适的位置上了
            return;
        }
        // 如果没有在合适的位置上，则交换
        std::swap(array[parent], array[child]);
        // 更新 child 和 parent
        child = parent;
        parent = (child - 1) / 2;
    }
}

// 下滤操作
void time_heap::percolate_down(int parent)
{
    int child = parent * 2 + 1; // hole 节点的左孩子
    while (child < size_)
    {
        // 先比较左右两个孩子的大小
        if (child + 1 <= size_ && array[child + 1]->expire < array[child]->expire)
        {
            // 如果右孩子小，就使用右孩子
            child++;
        }

        // 再跟父节点比较
        if (array[child]->expire < array[parent]->expire)
        {
            // 如果孩子更小，则交换
            std::swap(array[child], array[parent]);
            // 更新 parent 和 child 节点
            parent = child;
            child = parent * 2 + 1;
        }
        else
        {
            // 否则下滤完毕
            return;
        }
    }
}
