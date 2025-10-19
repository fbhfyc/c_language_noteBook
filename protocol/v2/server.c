#include "common.h"

message_queue_t g_message_queue;

// 工作线程函数
void* worker_thread(void* arg) {
    printf("工作线程启动: %ld\n", pthread_self());
    
    while (1) {
        message_t msg;
        if (dequeue_message(&g_message_queue, &msg) == 0) {
            printf("=== 工作线程 %ld 处理消息 ===\n", pthread_self());
            printf("客户端: %s:%d\n", 
                   inet_ntoa(msg.client_addr.sin_addr), 
                   ntohs(msg.client_addr.sin_port));
            printf("消息内容: ID=%u, Name=%s\n", 
                   ntohl(msg.packet.id), msg.packet.name);
            printf("接收时间: %ld\n", msg.receive_time);
            printf("==============================\n");
            
            // 发送响应
            char response[256];
            snprintf(response, sizeof(response), 
                    "Thread-%ld: 处理完成 ID=%u", 
                    pthread_self(), ntohl(msg.packet.id));
            send(msg.client_fd, response, strlen(response), 0);
            
            close(msg.client_fd);
        }
    }
    return NULL;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t workers[WORKER_THREAD_NUM];
    
    // 初始化消息队列
    if (init_queue(&g_message_queue, MAX_QUEUE_SIZE) != 0) {
        perror("初始化消息队列失败");
        exit(EXIT_FAILURE);
    }
    
    // 创建工作线程
    for (int i = 0; i < WORKER_THREAD_NUM; i++) {
        if (pthread_create(&workers[i], NULL, worker_thread, NULL) != 0) {
            perror("创建工作者线程失败");
            exit(EXIT_FAILURE);
        }
    }
    
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
    
    // 绑定和监听
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, 100) < 0) {  // 增大backlog
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("B进程(服务器)启动成功，监听端口 %d...\n", SERVER_PORT);
    printf("使用 %d 个工作线程，队列大小 %d\n", WORKER_THREAD_NUM, MAX_QUEUE_SIZE);
    
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }
        
        // 接收协议报文
        protocol_header_t packet;
        ssize_t bytes_received = recv(client_fd, &packet, sizeof(packet), 0);
        
        if (bytes_received == sizeof(packet) && validate_protocol_packet(&packet) == 0) {
            // 创建消息
            message_t msg;
            msg.client_fd = client_fd;
            msg.client_addr = client_addr;
            msg.packet = packet;
            msg.receive_time = time(NULL);
            
            // 将消息放入队列
            if (enqueue_message(&g_message_queue, &msg) == 0) {
                printf("新消息已加入队列，当前队列大小: %d/%d\n", 
                       g_message_queue.size, g_message_queue.capacity);
            } else {
                printf("队列已满，拒绝消息\n");
                const char* response = "服务器繁忙，请稍后重试";
                send(client_fd, response, strlen(response), 0);
                close(client_fd);
            }
        } else {
            printf("接收或验证协议报文失败\n");
            close(client_fd);
        }
    }
    
    // 清理资源（实际上不会执行到这里）
    destroy_queue(&g_message_queue);
    close(server_fd);
    return 0;
}
