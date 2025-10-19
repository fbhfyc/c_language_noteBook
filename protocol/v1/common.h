#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// 协议定义
#define MAGIC_NUMBER 0x1234ABCD  // 魔数，用于标识协议
#define PROTOCOL_VERSION 1       // 协议版本
#define MAX_NAME_LENGTH 32       // 名字最大长度

// 协议头
typedef struct {
    uint32_t magic;             // 魔数
    uint16_t version;           // 协议版本
    uint16_t length;            // 整个报文长度（包括头部和数据）
    uint32_t id;                // ID
    char name[MAX_NAME_LENGTH]; // 名称
} __attribute__((packed)) protocol_header_t;

// 创建协议报文
protocol_header_t* create_protocol_packet(uint32_t id, const char* name) {
    protocol_header_t* packet = (protocol_header_t*)malloc(sizeof(protocol_header_t));
    if (!packet) return NULL;
    
    packet->magic = htonl(MAGIC_NUMBER);
    packet->version = htons(PROTOCOL_VERSION);
    packet->id = htonl(id);
    packet->length = htons(sizeof(protocol_header_t));
    
    // 安全拷贝名称
    strncpy(packet->name, name, MAX_NAME_LENGTH - 1);
    packet->name[MAX_NAME_LENGTH - 1] = '\0';
    
    return packet;
}

// 验证协议报文
int validate_protocol_packet(const protocol_header_t* packet) {
    if (ntohl(packet->magic) != MAGIC_NUMBER) {
        printf("Invalid magic number: 0x%08X\n", ntohl(packet->magic));
        return -1;
    }
    
    if (ntohs(packet->version) != PROTOCOL_VERSION) {
        printf("Invalid protocol version: %d\n", ntohs(packet->version));
        return -1;
    }
    
    if (ntohs(packet->length) != sizeof(protocol_header_t)) {
        printf("Invalid packet length: %d\n", ntohs(packet->length));
        return -1;
    }
    
    return 0;
}

// 打印协议报文内容
void print_protocol_packet(const protocol_header_t* packet) {
    printf("=== 收到协议报文 ===\n");
    printf("魔数: 0x%08X\n", ntohl(packet->magic));
    printf("版本: %d\n", ntohs(packet->version));
    printf("长度: %d bytes\n", ntohs(packet->length));
    printf("ID: %u\n", ntohl(packet->id));
    printf("名称: %s\n", packet->name);
    printf("===================\n");
}

#endif
