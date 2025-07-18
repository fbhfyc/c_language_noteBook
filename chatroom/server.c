#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define TIME_SIZE 16 // 表示时间的字符串长度
#define IP_SIZE 16 // IP 字符串长度
#define BUF_SIZE 256 // 缓冲区大小
#define CLIENT_SIZE 8 // 允许的客户端数量
#define BACKLOG CLIENT_SIZE // listen 队列长度，等于允许的客户端数量
#define INVALID -1

struct CLIENT{
    int clientfd;
    struct sockaddr_in sockaddr;
    char ip[IP_SIZE];
    int port;
}clients[CLIENT_SIZE];

void init_clients(void)
{
    int i;

    for( i = 0; i < CLIENT_SIZE; i++ )
    {
        clients[i].clientfd = INVALID;
    }
}

void broadcast(char *msg)
{
    int i;

    for(i = 0; i<CLIENT_SIZE; i++)
    {
        if( clients[i].clientfd != INVALID )
        {
            write(clients[i].clientfd, msg, strlen(msg));
        }
    }
}

void stdmsg(int i, char *buffer, const char *msg)
{
    char curtime[TIME_SIZE];
    time_t curtime_t;
    struct tm *timeinfo;

    curtime_t = time(NULL);
    timeinfo = localtime(&curtime_t);
    strftime(curtime, TIME_SIZE, "%X", timeinfo);
    sprintf(buffer,"<%s %s:%d> %s",curtime,clients[i].ip,clients[i].port,msg);
}

void accept_connect(int listenfd)
{
    int connectfd,i;
    char buffer[BUF_SIZE];
    struct sockaddr_in clientaddr;
    socklen_t connectlen = sizeof(struct sockaddr_in);

    connectfd = accept( listenfd, (struct sockaddr_in *)&clientaddr, &connectlen);


    for( i = 0; i < CLIENT_SIZE; i++ )
    {
        if(clients[i].clientfd == INVALID)
        {
            clients[i].clientfd = connectfd;
            memcpy(&clients[i].sockaddr, &clientaddr, sizeof(clientaddr));
            clients[i].port = ntohs(clients[i].sockaddr.sin_port);
            inet_ntop(AF_INET, &clients[i].sockaddr.sin_addr, clients[i].ip, IP_SIZE);
            stdmsg(i,buffer,"login\n");
            printf("%s",buffer);
            broadcast(buffer);
            break;
        }
    }

    if (i == CLIENT_SIZE )
    {
        strcpy(buffer, "out of number\n");
        write(connectfd, buffer, strlen(buffer));
        close(connectfd);//所有操作利用buffer进行操作
    }
}


void chat(fd_set fdset)
{
    int sockfd, read_size, i;
    char read_buf[BUF_SIZE], send_buf[BUF_SIZE];

    for( i = 0; i < CLIENT_SIZE; i++ )
    {
        sockfd = clients[i].clientfd;

        if(sockfd != INVALID && FD_ISSET(sockfd,&fdset))
        {
            read_size = read(sockfd, read_buf, BUF_SIZE - 1);

            if(read_size == 0)
            {
                //connection lost
                close(sockfd);
                clients[i].clientfd = INVALID;
                stdmsg(i, send_buf, "logout\n");
                printf("%s\n",send_buf);
                broadcast(send_buf);

                continue;
            }
            else
            {
                read_buf[read_size] = '\0';
                stdmsg(i, send_buf, read_buf);
                printf("%s",send_buf);
                broadcast(send_buf);
            }
        }
    }
}

int socket_setup(int port)
{
    int rtn, listenfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sockaddr;

    bzero(&sockaddr, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    rtn = bind(listenfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    if (rtn == INVALID)
    {
        puts("bind error\n");
        exit(1);
    }

    if(listen(listenfd,BACKLOG) == INVALID)
    {
        puts("listen error\n");
        exit(1);
    }

    puts("service setup\n");
    return listenfd;
}

int main(int argc,const char *argv[])
{
    int maxfd;  
    int listenfd = socket_setup(atoi(argv[1]));

    fd_set fdset;

    init_clients();

    while(1)
    {
        FD_ZERO(&fdset);
        FD_SET(listenfd, &fdset);
        maxfd = listenfd;

        for(int i = 0; i < CLIENT_SIZE; i++)
        {
            if(clients[i].clientfd != INVALID)
            {
                FD_SET(clients[i].clientfd, &fdset);

                if(clients[i].clientfd > maxfd)
                {
                    maxfd = clients[i].clientfd;
                }
            }
         }

         select(maxfd + 1, &fdset, NULL, NULL, 0);

         if(FD_ISSET(listenfd, &fdset))
         {
            accept_connect(listenfd);
         }
         chat(fdset);
      }
    return 0;
}
