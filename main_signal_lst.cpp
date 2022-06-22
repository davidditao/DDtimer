// 使用「alarm信号」与「升序链表」处理非活动链接
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

#include "sort_timer_lst.h"
#include "util_timer.h"

#define FD_LIMIT 65535
#define MAX_EVENT_NUMBER 1024
#define TIMESLOT 5

static int pipefd[2];            // 管道
static sort_timer_lst timer_lst; // 升序链表
static int epollfd = 0;

int setnonblocking(int fd);           // 设置非阻塞IO
void addfd(int epollfd, int fd);      // 添加 listenfd 到 epoll 中
void sig_handler(int sig);            // 信号处理函数
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
    bzero(&servaddr, sizeof(servaddr));
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

    /* 使用 socketpair，和 pipe 的区别是：socketpair 是全双工。
       将管道注册到 epoll 上，当有 alarm 信号到来时，会往管道中写输出。
       epoll 监听到管道中有数据，会执行相应的定时事件回调 */
    ret = socketpair(PF_UNIX, SOCK_STREAM, 0, pipefd);
    if (ret < 0)
    {
        perror("socketpair()");
        return 1;
    }
    setnonblocking(pipefd[1]); // 写端
    addfd(epollfd, pipefd[0]); // 读端，需要监听事件

    // 添加信号
    addsig(SIGALRM);
    addsig(SIGTERM);

    bool stop_server = false;

    client_data *users = new client_data[FD_LIMIT];
    bool timeout = false; // 标记有定时任务需要处理
    alarm(TIMESLOT);      // 定时，TIMESLOT 秒后发送 alarm 信号

    printf("Server start run... \n");
    while (!stop_server)
    {
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if ((number < 0) && (errno != EINTR))
        {
            printf("epoll failure\n");
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
            // 处理信号, alarm信号会向管道中写数据
            else if ((sockfd == pipefd[0]) && (events[i].events && EPOLLIN))
            {
                printf("sockfd = pipefd[0]\n");
                int sig;
                char signals[1024];
                ret = recv(pipefd[0], signals, sizeof(signals), 0); // 获取管道中的数据
                if (ret == -1)
                {
                    // error, handle the error
                    continue;
                }
                else if (ret == 0)
                {
                    // 另一端已经关闭
                    continue;
                }
                else
                {
                    // 成功读取数据
                    for (int j = 0; j < ret; j++)
                    {
                        printf("signals[i] = %d \n", (int)signals[j]);
                        switch (signals[j])
                        {
                        case SIGALRM:
                        {
                            /* timeout 标记有定时任务需要处理，但不立即处理定时任务。
                               因为定时任务的优先级不是很高，我们优先处理其他更重要的任务*/
                            timeout = true;
                            break;
                        }
                        case SIGTERM:
                        {
                            // 停止服务器
                            stop_server = true;
                        }
                        }
                    }
                }
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

        // 最后处理定时事件，因为IO事件有更高的优先级。当然，这样做将导致定时任务不能精确地按照预期的时间执行
        if (timeout)
        {
            timer_handler();
            timeout = false;
        }
    } // end of while

    close(listenfd);
    close(pipefd[1]);
    close(pipefd[0]);
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

// 信号处理函数
void sig_handler(int sig)
{
    int save_errno = errno;
    int msg = sig;
    send(pipefd[1], (char *)&msg, 1, 0); // 写端口发送数据
    errno = save_errno;
}

// 添加信号
void addsig(int sig)
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = sig_handler;       // 设置信号处理函数
    sa.sa_flags |= SA_RESTART;         // 如果信号中断了当前系统调用，让系统调用函数恢复运行
    sigfillset(&sa.sa_mask);           // 屏蔽其他信号
    if (sigaction(sig, &sa, NULL) < 0) // 捕获信号
    {
        perror("sigaction()");
        return;
    }
}

// 定时器处理任务
void timer_handler()
{
    printf("Entry the timer_handler() \n");
    // 处理定时任务实际上是调用tick函数
    timer_lst.tick();
    alarm(TIMESLOT);
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