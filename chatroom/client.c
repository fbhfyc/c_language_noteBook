#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 256
#define STDIN 0
#define STDOUT 1
#define INVALID -1

int socket_setup(const char *serv_ip, int serv_port)
{
    int rtn,sockfd;
    struct sockaddr_in sockaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&sockaddr,sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(serv_port);
    inet_pton(AF_INET, serv_ip, &sockaddr.sin_addr);

    rtn = connect(sockfd,(struct sockaddr *)&sockaddr, sizeof(sockaddr));

    if (rtn == INVALID)
    {
        puts("connection failure\n");
	printf("[%s],[%d]\n",serv_ip,serv_port);
        exit(1);
    }
    else
    {
        puts("connection successful\n");
        return sockfd;
    }
}

int main(int argc, const char *argv[])
{
    int i,read_size, sockfd = socket_setup(argv[1],atoi(argv[2]));
    char buffer[BUF_SIZE];
    fd_set fdset;

    while (1)
    {
        FD_ZERO(&fdset);
        FD_SET(STDIN, &fdset);
        FD_SET(sockfd, &fdset);
        select(sockfd + 1, &fdset, NULL, NULL, 0);

        if( FD_ISSET( sockfd, &fdset ) )
        {
            read_size = read(sockfd, buffer, BUF_SIZE);
            write(STDOUT, buffer, read_size);

            if(read_size == 0)
            {
                puts("server close");
                exit(1);
            }
        }

        if(FD_ISSET(STDIN, &fdset))
        {
            read_size = read(STDIN, buffer, BUF_SIZE);
            write(sockfd, buffer, read_size);
        }
    }

    return 0;
}
