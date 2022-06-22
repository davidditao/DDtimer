#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/**
 * socket 选项设置定时
 * ~/my_code/mytimer$ ./timer_socket 127.0.0.1 1997
 * accepting timeout, process timeout logic 
 */

/* 超时连接函数 */
int timeout_connect(const char *ip, int port, int time)
{
    int ret = 0;

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if(listenfd < 0){
        perror("socket()");
        return -1;
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    ret = bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(ret < 0){
        perror("bind()");
        return -1;
    }

    ret = listen(listenfd, 1024);
    if(ret < 0){
        perror("listen()");
        return -1;
    }

    /* 通过选项 SO_RCVTIMEO 和 SO_SNDTIMEO 所设置的超时时间的类型是 timeval,
    这和 select 系统调用的超时参数类型相同*/
    struct timeval timeout;
    timeout.tv_sec = time; // 秒
    timeout.tv_usec = 0;   // 毫秒

    socklen_t len = sizeof(timeout);
    ret = setsockopt(listenfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, len);
    assert(ret != -1);

    int connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
    if(connfd == -1){
        if(errno == EWOULDBLOCK){
            // 如果超时
            printf("accepting timeout, process timeout logic \n");
            return -1;
        }
        perror("accept()");
        return -1;
    }
    return connfd;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);

    int sockfd = timeout_connect(ip, port, 3);
    if (sockfd < 0)
    {
        return 1;
    }
    return 0;
}
