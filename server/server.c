#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "rgb2bmp.h"

int newsock;

// 线程的任务函数 --> 专门发送信息给客户端
// 线程的任务函数 --> 专门发送信息给客户端
void *sendmsgtoclient(void *arg)
{
    char sbuf[100];
    while (1)
    {
        bzero(sbuf, 100);

        // 提示用户可用的选项
        printf("可用的选项:\n");
        printf("1234 - 打开监控画面\n");
        printf("1235 - 关闭监控画面\n");
        printf("1236 - 监控画面截取\n");
        printf("1237 - 发送当前监控画面到服务器\n");
        printf("请输入要发送给客户端的信息: ");

        fgets(sbuf, 100, stdin);
        sbuf[strcspn(sbuf, "\n")] = '\0'; // 去除输入中的换行符

        if (strcmp(sbuf, "1234") == 0)
        {
            write(newsock, sbuf, strlen(sbuf) + 1);
            printf("消息 '1234' 已发送给客户端--打开监控画面\n");
        }
        else if (strcmp(sbuf, "1235") == 0)
        {
            write(newsock, sbuf, strlen(sbuf) + 1);
            printf("消息 '1235' 已发送给客户端--关闭监控画面\n");
        }
        else if (strcmp(sbuf, "1236") == 0)
        {
            write(newsock, sbuf, strlen(sbuf) + 1);
            printf("消息 '1236' 已发送给客户端--监控画面截取\n");
            break;
        }
        else if (strcmp(sbuf, "1237") == 0)
        {
            write(newsock, sbuf, strlen(sbuf) + 1);
            printf("消息 '1237' 已发送给客户端--发送当前监控画面到服务器\n");
            break;
        }
        else
        {
            write(newsock, sbuf, strlen(sbuf) + 1);
            printf("消息 '%s' 已发送给客户端\n", sbuf);
        }
    }
    return NULL;
}


// 线程的任务函数 --> 专门接收客户端发送的文件
void *recvfilefromclient(void *arg)
{
    while (1)
    {
        int filesize = 0, received = 0, r;
        unsigned char buf[1024];
	
        
        // 接收文件大小
        r = read(newsock, &filesize, sizeof(filesize));
        if (r <= 0) {
            perror("read filesize error");
            break;
        }
        printf("文件大小: %d 字节\n", filesize);

        int fd = open("received_file.bmp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            perror("open file error");
           break;
        }

        // 接收文件内容
        while (received < filesize)
        {
            r = read(newsock, buf, sizeof(buf));
            if (r <= 0) {
                perror("read file data error");
                break;
            }
            write(fd, buf, r);
            received += r;
        }

        printf("文件接收完毕，接收了 %d 字节\n", received);

	// lseek(fd, 0, SEEK_SET);
	// unsigned char colorbuf[3*640*480];
    // read(fd, colorbuf, sizeof(colorbuf));
	// imageBmp(colorbuf, 640, 480, "1.bmp");


        close(fd);
	
    }
    return NULL;
}

int main()
{
    int tcpsock, ret;
    pthread_t send_thread, recv_thread;
    
    // 定义ipv4地址体变量存放需要绑定的ip和端口号
    struct sockaddr_in bindaddr;
    bzero(&bindaddr, sizeof(bindaddr));
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = inet_addr("192.168.100.100"); // 服务器的IP
    bindaddr.sin_port = htons(2000); // 服务器的端口号
    
    // 定义ipv4地址体变量存放客户端的ip和端口号
    struct sockaddr_in boyaddr;
    int addrsize = sizeof(boyaddr);
    
    // 创建tcp套接字
    tcpsock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpsock == -1) {
        perror("创建tcp套接字失败!\n");
        return -1;
    }
    
    // 绑定服务器自己的ip和端口号
    ret = bind(tcpsock, (struct sockaddr *)&bindaddr, sizeof(bindaddr));
    if (ret == -1) {
        perror("绑定ip端口失败了!\n");
        return -1;
    }
    
    // 监听
    ret = listen(tcpsock, 5);
    if (ret == -1) {
        perror("监听失败!\n");
        return -1;
    }
    printf("旧的套接字(监听套接字)是:%d\n", tcpsock);
    printf("服务器的代码阻塞在accept位置了!\n");
    
    // 接收客户端的连接请求
    newsock = accept(tcpsock, (struct sockaddr *)&boyaddr, &addrsize);
    if (newsock == -1) {
        perror("接收连接请求失败了!\n");
        return -1;
    }
    printf("有一个客户端连接成功了，产生的新套接字(已连接套接字)是:%d\n", newsock);
    
    // 创建一个子线程专门发送信息
    pthread_create(&send_thread, NULL, sendmsgtoclient, NULL);
    
    // 创建一个子线程专门接收文件
    pthread_create(&recv_thread, NULL, recvfilefromclient, NULL);
    
    // 主线程等待子线程结束
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);
    
    // 关闭套接字
    close(tcpsock);
    close(newsock);
    return 0;
}
