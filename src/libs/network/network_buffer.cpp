#include "packet.h"
#include "network_buffer.h"

#include <cstdlib>
#include <iostream>
#include <cstring>

NetworkBuffer ::NetworkBuffer(const unsigned size)
{
    _bufferSize = size;
    _beginIndex = 0;
    _endIndex = 0;
    _dataSize = 0;
    _buffer = new char[_bufferSize];
}

NetworkBuffer::~NetworkBuffer()
{
    if (_buffer != nullptr)
        delete[] _buffer;
}

bool NetworkBuffer::HasData() const
{
    if (_dataSize <= 0)
        return false;

    // 数据至少要有一个协议头大小
    if (_dataSize < sizeof(PacketHead))
        return false;
    return true;
}

unsigned int NetworkBuffer::GetEmptySize()
{
    return _bufferSize - _dataSize;
}

unsigned int NetworkBuffer::GetWriteSize() const
{
    if (_beginIndex <= _endIndex)
    {
        return _bufferSize - _endIndex;
    }
    else
    {
        return _beginIndex - _endIndex;
    }
}

unsigned int NetworkBuffer::GetReadSize() const
{
    if (_dataSize <= 0)
        return 0;

    if (_beginIndex < _endIndex)
    {
        return _endIndex - _beginIndex;
    }
    else
    {
        return _bufferSize - _beginIndex;
    }
}

void NetworkBuffer::FillDate(unsigned int size)
{
    _dataSize += size;

    if ((_bufferSize - _endIndex) <= size)
    {
        size -= _bufferSize - _endIndex;
        _endIndex = 0;
    }

    _endIndex += size;
}

void NetworkBuffer::RemoveDate(unsigned int size)
{
    _dataSize -= size;

    if ((_beginIndex + size) >= _bufferSize)
    {
        size -= _bufferSize - _beginIndex;
        _beginIndex = 0;
    }
    _beginIndex += size;
}

void NetworkBuffer::ReAllocBuffer()
{
    Buffer::ReAllocBuffer(_dataSize);
}

////////////////////////////////////RecvNetworkBuffer//////////////////////////////////////////////
RecvNetworkBuffer::RecvNetworkBuffer(const unsigned int _size) : NetworkBuffer(_size)
{
}

void RecvNetworkBuffer::Dispose()
{
}

int RecvNetworkBuffer::GetBuffer(char *&pBuffer) const
{
    pBuffer = _buffer + _endIndex;
    return GetWriteSize();
}

Packet *RecvNetworkBuffer::GetPacket()
{
    // 数据长度不足"包长度"
    if (_dataSize < sizeof(TotalSizeType))
    {
        return nullptr;
    }

    // 1.读出整体的长度
    unsigned short totalSize;
    MemcpyFromBuffer(reinterpret_cast<char *>(&totalSize), sizeof(TotalSizeType));

    // 协议长度不够 等待
    if (_dataSize < totalSize)
    {
        return nullptr;
    }

    RemoveDate(sizeof(TotalSizeType));

    // 2.读出PacketHead
    PacketHead head;
    MemcpyFromBuffer(reinterpret_cast<char *>(&head), sizeof(PacketHead));
    RemoveDate(sizeof(PacketHead));

    // 3.读出 协议
    Packet *pPacket = new Packet(head.MsgId);
    const auto dataLength = totalSize - sizeof(PacketHead) - sizeof(TotalSizeType);
    while (pPacket->GetTotalSize() < dataLength)
    {
        pPacket->ReAllocBuffer();
    }

    MemcpyFromBuffer(pPacket->GetBuffer(), dataLength);
    pPacket->FillData(dataLength);
    RemoveDate(dataLength);

    return pPacket;
}

void RecvNetworkBuffer::MemcpyFromBuffer(char *pVoid, const unsigned int size)
{
    const auto readSize = GetReadSize();
    // endIndex小于beginIndex
    if (readSize < size)
    {
        // 1.copy尾部数据
        ::memcpy(pVoid, _buffer + _beginIndex, readSize);

        // 2.copy头部数据
        ::memcpy(pVoid + readSize, _buffer, size - readSize);
    }
    else
    {
        ::memcpy(pVoid, _buffer + _beginIndex, size);
    }
}

////////////////////////////////////SendNetworkBuffer//////////////////////////////////////////////
SendNetworkBuffer::SendNetworkBuffer(const unsigned int _size) : NetworkBuffer(_size)
{
}

void SendNetworkBuffer::Dispose()
{
}

int SendNetworkBuffer::GetBuffer(char *&pBuffer) const
{
    if (_dataSize <= 0)
    {
        pBuffer = nullptr;
        return 0;
    }

    if (_beginIndex < _endIndex)
    {
        pBuffer = _buffer + _beginIndex;
        return _endIndex - _beginIndex;
    }
    else
    {
        pBuffer = _buffer + _beginIndex;
        return _bufferSize - _beginIndex;
    }
}

void SendNetworkBuffer::AddPacket(Packet *pPacket)
{
    const auto dataLength = pPacket->GetDataLength();
    TotalSizeType totalSize = dataLength + sizeof(PacketHead) + sizeof(TotalSizeType);

    // SendNetworkBuffer长度不够，扩容
    while (GetEmptySize() < totalSize)
    {
        ReAllocBuffer();
    }

    // 1.发送整体长度
    MemcpyToBuffer(reinterpret_cast<char *>(&totalSize), sizeof(TotalSizeType));

    // 2.头部
    PacketHead head;
    head.MsgId = pPacket->GetMsgId();
    MemcpyToBuffer(reinterpret_cast<char *>(&head), sizeof(PacketHead));

    // 3.数据
    MemcpyToBuffer(pPacket->GetBuffer(), pPacket->GetDataLength());
}

void SendNetworkBuffer::MemcpyToBuffer(char *pVoid, const unsigned int size)
{
    const auto writeSize = GetWriteSize();

    // end到buffer未空间不足
    if (writeSize < size)
    {
        // 1.copy到尾部
        ::memcpy(_buffer + _endIndex, pVoid, writeSize);
        // 2.copy到头部
        ::memcpy(_buffer, pVoid + writeSize, size - writeSize);
    }
    else
    {
        ::memcpy(_buffer + _endIndex, pVoid, size);
    }

    FillDate(size);
}