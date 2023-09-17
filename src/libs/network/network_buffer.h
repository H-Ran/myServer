#pragma once

#include "base_buffer.h"

#if TestNetwork
#define DEFAULT_SEND_BUFFER_SIZE 10
#define DEFAULT_RECV_BUFFER_SIZE 10
#else
// 默认大小 128KB
#define DEFAULT_SEND_BUFFER_SIZE 1024 * 128
#define DEFAULT_RECV_BUFFER_SIZE 1024 * 128
#endif

class Packet;

// 存储协议总长度的类型
using TotalSizeType = unsigned short;

class NetworkBuffer : public Buffer
{
public:
    explicit NetworkBuffer(const unsigned int size);

    virtual ~NetworkBuffer();

    bool HasData() const;

    // 包括环的头与环的尾一共的空字节数
    unsigned int GetEmptySize() override;

    // 当前可写长度
    unsigned int GetWriteSize() const;

    // 当前可读长度
    unsigned int GetReadSize() const;

    void FillDate(unsigned int size);
    void RemoveDate(unsigned int size);
    void ReAllocBuffer();
};