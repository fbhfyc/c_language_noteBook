#include "common.h"

#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    
    // 创建TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    
    // 绑定socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    // 开始监听
    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("B进程(服务器)启动成功，监听端口 %d...\n", SERVER_PORT);
    
    while (1) {
        // 接受客户端连接
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }
        
        printf("新的客户端连接: %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        // 接收数据
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(protocol_header_t), 0);
        if (bytes_received < 0) {
            perror("recv failed");
            close(client_fd);
            continue;
        }
        
        if (bytes_received == 0) {
            printf("客户端断开连接\n");
            close(client_fd);
            continue;
        }
        
        if (bytes_received != sizeof(protocol_header_t)) {
            printf("接收数据长度不匹配: %ld != %ld\n", 
                   bytes_received, sizeof(protocol_header_t));
            close(client_fd);
            continue;
        }
        
        // 验证协议报文
        protocol_header_t* packet = (protocol_header_t*)buffer;
        if (validate_protocol_packet(packet) == 0) {
            // 打印报文信息
            print_protocol_packet(packet);
        } else {
            printf("协议报文验证失败\n");
        }
        
        // 发送响应
        const char* response = "报文接收成功";
        send(client_fd, response, strlen(response), 0);
        
        close(client_fd);
    }
    
    close(server_fd);
    return 0;
}
