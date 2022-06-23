// 使用「epoll定时」与「升序链表」处理非活动链接
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>

#include "sort_timer_lst.cpp"
#include "util_timer.h"

#define FD_LIMIT 65535
#define MAX_EVENT_NUMBER 1024
#define TIMEOUT 5000 // epoll
#define TIMESLOT 5

static sort_timer_lst timer_lst; // 升序链表
static int epollfd = 0;

int setnonblocking(int fd);           // 设置非阻塞IO
void addfd(int epollfd, int fd);      // 添加 listenfd 到 epoll 中
void addsig(int sig);                 // 添加信号
void timer_handler();                 // 定时器处理任务
void cb_func(client_data *user_data); // 定时器回调函数

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("usage:%s ip_address port_number\n", basename(argv[0]));
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    int ret = 0;
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &servaddr.sin_addr);
    servaddr.sin_port = htons(port);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        perror("socket()");
        return 1;
    }

    // 设置复用端口
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    ret = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret < 0)
    {
        perror("bind()");
        return 1;
    }

    ret = listen(listenfd, 1024);
    if (ret < 0)
    {
        perror("listen()");
        return 1;
    }

    epoll_event events[MAX_EVENT_NUMBER]; // 用于存储 epoll 监听到的事件
    int epollfd = epoll_create(5);
    if (epollfd < 0)
    {
        perror("epoll_create()");
        return 1;
    }

    // 添加 listenfd 到 epoll 中
    addfd(epollfd, listenfd);

    bool stop_server = false;

    client_data *users = new client_data[FD_LIMIT];

    int timeout = TIMEOUT;
    time_t start = time(NULL);
    time_t end = time(NULL);

    printf("Server start run... \n");
    while (!stop_server)
    {
        printf("\n");
        printf("the timeout is now %d mil-seconds\n", timeout);

        start = time(NULL); // 记录循环起始时间

        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, timeout);
        if ((number < 0) && (errno != EINTR))
        {
            printf("epoll failure\n");
        }
        // 如果 epoll_wait 成功返回 0，则说明超时时间到，而且没有别的事件到达。
        if (number == 0)
        {
            // 超时
            timeout = TIMEOUT; // 重置超时时间
            timer_handler();   // 没有别的事件到达，可以直接处理超时事件
            continue;          // 处理完后直接返回
        }

        printf("\n");
        for (int i = 0; i < number; i++)
        {
            int sockfd = events[i].data.fd;
            // 处理新用户连接
            if (sockfd == listenfd)
            {
                printf("sockfd = new client\n");
                struct sockaddr_in cliaddr;
                socklen_t clilen = sizeof(cliaddr);
                int connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
                if (connfd < 0)
                {
                    perror("accept()");
                    return 1;
                }

                addfd(epollfd, connfd);

                // 保存已经连接的用户信息
                users[connfd].address = cliaddr;
                users[connfd].sockfd = connfd;

                util_timer *timer = new util_timer(); // 因为数据较大，必须在堆上开辟空间，在栈上开辟会造成栈空间不足
                timer->user_data = &users[connfd];
                timer->cb_func = cb_func;
                time_t curtime = time(NULL);
                timer->expire = curtime + 3 * TIMESLOT; // 给该连接设置定时时间

                users[connfd].timer = timer;

                timer_lst.add_timer(timer); // 加入到链表中
            }
            // 处理客户端消息
            else if (events[i].events & EPOLLIN)
            {
                printf("sockfd = client_data\n");
                memset(users[sockfd].buf, '\0', BUFFER_SIZE);
                ret = recv(sockfd, users[sockfd].buf, BUFFER_SIZE - 1, 0);
                printf("get %d bytes of client data from %d: %s\n", ret, sockfd, users[sockfd].buf);

                util_timer *timer = users[sockfd].timer;
                if (ret < 0)
                {
                    // 如果发生错误，关闭连接，移除其对应的定时器
                    if (errno != EAGAIN)
                    {
                        cb_func(&users[sockfd]); // 关闭连接
                        if (timer != nullptr)
                        {
                            timer_lst.del_timer(timer);
                        }
                    }
                }
                else if (ret == 0)
                {
                    // 如果另一端已经关闭
                    cb_func(&users[sockfd]); // 关闭连接
                    if (timer != nullptr)
                    {
                        timer_lst.del_timer(timer);
                    }
                }
                else
                {
                    // 如果某个连接上有数据可读，则调整对应定时器
                    if (timer)
                    {
                        time_t curtime = time(NULL);
                        timer->expire = curtime + 3 * TIMESLOT;
                        printf("adjust timer once\n");
                        timer_lst.adjust_timer(timer);
                    }
                }
            }
            else
            {
                // others
            }
        } // end of for

        end = time(NULL); // 记录循环结束时间

        // 本轮循环的调用时间是 (end - start)*1000 ms，我们需要将定时时间 timeout 减去这段时间，以获得下次 epoll_wait 调用时的超时参数
        timeout = timeout - (end - start) * 1000;

        // 如果 timeout <= 0, 说明定时事件在本轮循环的时候产生了，此时我们需要马上处理定时任务，并重置定时时间
        if (timeout <= 0)
        {
            timer_handler();
            timeout = TIMEOUT;
        }
    } // end of while

    close(listenfd);
    delete[] users;

    printf("exit!\n");

    return 0;
}

// 设置非阻塞IO
int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);      // 获取当前文件描述符的状态
    int new_option = old_option | O_NONBLOCK; // 在旧状态上添加非阻塞选项
    fcntl(fd, new_option);
    return old_option;
}

// 添加 listenfd 到 epoll 中
void addfd(int epollfd, int fd)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET; // 边缘触发模式
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

// 定时器处理任务
void timer_handler()
{
    printf("Entry the timer_handler() \n");
    // 处理定时任务实际上是调用tick函数
    timer_lst.tick();
}

// 定时器回调函数
void cb_func(client_data *user_data)
{
    // 如果该连接超时，则将该连接删除
    epoll_ctl(epollfd, EPOLL_CTL_DEL, user_data->sockfd, 0);
    close(user_data->sockfd);
    printf("cb_func : close fd %d\n", user_data->sockfd);
    sleep(1);
}
