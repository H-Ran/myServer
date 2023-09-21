#include "packet.h"
#include <iostream>
#include <cstring>

Packet::Packet()
{
    _msgId = 0;
    CleanBuffer();

    _bufferSize = DEFAULT_PACKET_BUFFER_SIZE;
    _beginIndex = 0;
    _endIndex = 0;
    _buffer = new char[_bufferSize];
}

Packet::Packet(const int msgId)
{
    _msgId = msgId;
    CleanBuffer();

    _bufferSize = DEFAULT_PACKET_BUFFER_SIZE;
    _beginIndex = 0;
    _endIndex = 0;
    _buffer = new char[_bufferSize];
}

Packet::~Packet()
{
    CleanBuffer();
}

void Packet::CleanBuffer()
{
    if (_buffer != nullptr)
        delete[] _buffer;

    _beginIndex = 0;
    _endIndex = 0;
    _bufferSize = 0;
}
// 调用Dispose()后,都会delete调用析构函数
// 无需手动清除buffer
void Packet::Dispose()
{
    _msgId = 0;
    _beginIndex = 0;
    _endIndex = 0;
}
