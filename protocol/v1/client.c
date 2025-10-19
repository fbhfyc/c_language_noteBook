#include "common.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    
    // 创建TCP socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("invalid address");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    
    // 连接服务器
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connection failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("A进程(客户端)连接服务器成功\n");
    
    // 创建协议报文
    protocol_header_t* packet = create_protocol_packet(1001, "测试用户");
    if (!packet) {
        printf("创建协议报文失败\n");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    
    // 发送协议报文
    ssize_t bytes_sent = send(sock_fd, packet, sizeof(protocol_header_t), 0);
    if (bytes_sent < 0) {
        perror("send failed");
    } else if (bytes_sent != sizeof(protocol_header_t)) {
        printf("发送数据不完整: %ld != %ld\n", bytes_sent, sizeof(protocol_header_t));
    } else {
        printf("协议报文发送成功\n");
        
        // 接收服务器响应
        char response[100];
        ssize_t bytes_received = recv(sock_fd, response, sizeof(response) - 1, 0);
        if (bytes_received > 0) {
            response[bytes_received] = '\0';
            printf("服务器响应: %s\n", response);
        }
    }
    
    // 清理资源
    free(packet);
    close(sock_fd);
    
    return 0;
}
